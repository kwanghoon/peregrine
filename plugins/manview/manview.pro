QT       -= gui

TARGET = manview
TEMPLATE = lib

DEFINES += MANVIEW_LIBRARY

SOURCES += manview.cpp

INCLUDEPATH += ../../plugin-sdk

HEADERS += manview.h\
        manview_global.h\
        ../../plugin-sdk/peregrine-plugin-sdk.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}