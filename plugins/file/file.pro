#-------------------------------------------------
#
# Project created by QtCreator 2016-07-28T20:47:50
#
#-------------------------------------------------

QT       -= gui

TARGET = file
TEMPLATE = lib

DEFINES += FILE_LIBRARY

SOURCES += file.cpp

INCLUDEPATH += ../../plugin-sdk

HEADERS += file.h\
        file_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# boost
INCLUDEPATH	+= $$(BOOST_ROOT)
LIBS += -L$$(BOOST_ROOT)/stage/lib

