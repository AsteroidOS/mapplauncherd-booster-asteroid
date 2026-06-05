TEMPLATE = app

TARGET = booster-asteroid-qt6
QT += qml quick concurrent
qml.files = qml-qt6/preload.qml

CONFIG += qdeclarative-boostable

target.path = /usr/libexec/mapplauncherd/
qml.path = /usr/share/$${TARGET}/

service.path = /usr/lib/systemd/user/
service.files = data/$${TARGET}.service

INSTALLS += target qml service

LIBS += -lapplauncherd -lmdeclarativecache6 -lEGL
INCLUDEPATH += /usr/include/applauncherd/

SOURCES += src/qmlbooster.cpp src/eventhandler.cpp
HEADERS += src/qmlbooster.h src/eventhandler.h
OTHER_FILES += qml-qt6/preload.qml

