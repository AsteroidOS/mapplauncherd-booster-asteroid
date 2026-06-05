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

# Resolve all relocations at startup so they do not turn into private dirty
# pages within boosted applications. A little extra initial memory in the
# supervisor for a lot of CoW-shared pages per app.
QMAKE_LFLAGS += -Wl,-z,now
INCLUDEPATH += /usr/include/applauncherd/

SOURCES += src/qmlbooster.cpp src/eventhandler.cpp
HEADERS += src/qmlbooster.h src/eventhandler.h
OTHER_FILES += qml-qt6/preload.qml

