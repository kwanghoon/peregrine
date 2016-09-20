QT       -= gui

TARGET = calculator
TEMPLATE = lib

DEFINES += CALCULATOR_LIBRARY

SOURCES += calculator.cpp

INCLUDEPATH += ../../plugin-sdk

HEADERS += calculator.h\
        calculator_global.h\
        ../../plugin-sdk/peregrine-plugin-sdk.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
