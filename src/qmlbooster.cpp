/***************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of applauncherd
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QFileInfo>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlError>
#include <QQuickView>
#include <QUrl>
#include <QtGlobal>
#include <QtQml>

#include <EGL/egl.h>

#include "qmlbooster.h"
#include "connection.h"
#include "logger.h"
#include "daemon.h"
#include <MDeclarativeCache>

const string QMLBooster::m_boosterType = "asteroid-qt6";

// Run the preload through an Asynchronous QQmlComponent so the booster's
// main thread is free to receive an invoker connection while the compile
// runs on Qt's loader thread. The component is kept alive after create()
// so its compiled bytecode cache stays around until the app starts.
class QMLBoosterData : public QObject
{
    Q_OBJECT
public:
    QMLBoosterData()
        : QObject(), engine(0), component(0)
    {
    }

    ~QMLBoosterData()
    {
        if (component && component->isLoading())
            Logger::logInfo("QMLBooster: Preload compilation aborted.");
        delete component;
    }

    void beginPreload(QQmlEngine *e, const QUrl &url)
    {
        engine = e;
        source = url;
        Logger::logInfo("QMLBooster: Initiate asynchronous preload.");
        component = new QQmlComponent(engine, source, QQmlComponent::Asynchronous);
        if (component->isLoading()) {
            QObject::connect(component, &QQmlComponent::statusChanged,
                             this, &QMLBoosterData::statusChanged);
        } else {
            statusChanged();
        }
    }

private slots:
    void statusChanged()
    {
        if (component->isError()) {
            Logger::logError("QMLBooster: Preload component failed to load:");
            foreach (const QQmlError &e, component->errors())
                Logger::logError("QMLBooster:    %s", e.toString().toLatin1().constData());
        } else {
            QQmlContext context(engine);
            QObject *obj = component->create(&context);
            if (obj)
                obj->setParent(this);
            else
                Logger::logError("QMLBooster: Preload object creation failed");
            delete obj;
        }
    }

private:
    QQmlEngine *engine;
    QQmlComponent *component;
    QUrl source;
};

QMLBooster::QMLBooster()
    : Booster(), data(new QMLBoosterData)
{
}

QMLBooster::~QMLBooster()
{
    delete data;
}

const string & QMLBooster::boosterType() const
{
    return m_boosterType;
}

bool QMLBooster::preload()
{
    QQuickView *view = MDeclarativeCache::populate();

    QString file = "/usr/share/booster-";
    file += boosterType().c_str();
    file += "/preload.qml";

    data->beginPreload(view->engine(), QUrl::fromLocalFile(file));

    return true;
}

bool QMLBooster::receiveDataFromInvoker(int socketFd)
{
    // Use the default implementation if in boot mode
    // (it won't require QApplication running).

    if (bootMode())
    {
        return Booster::receiveDataFromInvoker(socketFd);
    }
    else
    {
        // Setup the conversation channel with the invoker.
        setConnection(new Connection(socketFd));

        EventHandler handler(this);
        handler.runEventLoop();

        // An invoker is here -- stop any in-flight preload so we do not
        // compete with the app that is about to start. The destructor
        // aborts the async QQmlComponent if it is still loading.
        delete data;
        data = 0;

        if (!connection()->connected())
        {
            return false;
        }

        // Receive application data from the invoker
        if(!connection()->receiveApplicationData(appData()))
        {
            connection()->close();
            return false;
        }

        // Close the connection if exit status doesn't need
        // to be sent back to invoker
        if (!connection()->isReportAppExitStatusNeeded())
        {
            connection()->close();
        }

        return true;
    }
}


void QMLBooster::preinit()
{
    QString appName = QFileInfo(m_appData->argv()[0]).fileName();

    QString appClass = appName.left(1).toUpper();
    if (appName.length() > 1)
        appClass += appName.right(appName.length() - 1);

    // char* app_name = qstrdup(appName.toLatin1().data());
    // QApplication::setAppName(app_name);

    // char* app_class = qstrdup(appClass.toLatin1().data());
    // QApplication::setAppClass(app_class);
}

int main(int argc, char **argv)
{
    // Resolve the EGL/GLES backing libraries in the supervisor so workers
    // forked by Daemon::run() inherit them. eglBindAPI does not create a
    // wayland connection -- eglGetDisplay would, and that would not survive
    // the fork.
    (void)eglBindAPI(EGL_OPENGL_ES_API);

    QMLBooster *booster = new QMLBooster;
    Daemon d(argc, argv);
    d.run(booster);
    return 0;
}

#include "qmlbooster.moc"

