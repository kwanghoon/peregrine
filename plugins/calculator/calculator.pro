QT       -= gui

TARGET = calculator
TEMPLATE = lib

DEFINES += CALCULATOR_LIBRARY

SOURCES += calculator.cpp

INCLUDEPATH += ../../plugin-sdk \
    ../../thirdparty

HEADERS += calculator.h\
        calculator_global.h\
        ../../plugin-sdk/peregrine-plugin-sdk.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32-msvc* {
    QMAKE_CXXFLAGS += /bigobj
}
