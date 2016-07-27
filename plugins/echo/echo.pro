#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T01:55:02
#
#-------------------------------------------------

QT       -= gui

TARGET = echo
TEMPLATE = lib

DEFINES += ECHO_LIBRARY

SOURCES += echo.cpp

INCLUDEPATH += ../../plugin-sdk

HEADERS += echo.h\
        echo_global.h\
        ../../plugin-sdk/peregrine-plugin-sdk.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
