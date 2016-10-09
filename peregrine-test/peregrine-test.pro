QT       += testlib
QT       -= gui

TARGET = peregrine-test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp \	
    peregrinetest.cpp
	
HEADERS += \
    pereginetest.h

# boost
INCLUDEPATH += $$(BOOST_ROOT)

win32-msvc* {
    QMAKE_CXXFLAGS += /bigobj
}
