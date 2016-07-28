#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T20:47:50
#
#-------------------------------------------------

QT       -= gui

TARGET = website
TEMPLATE = lib

DEFINES += WEBSITE_LIBRARY

SOURCES += website.cpp

INCLUDEPATH += ../../plugin-sdk

HEADERS += website.h\
        website_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
