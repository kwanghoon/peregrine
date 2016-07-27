QT += core gui \
    xml \
    quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = peregrine-desktop
TEMPLATE = app

SOURCES += main.cpp\
        launcherwindow.cpp \
    actionselectdialog.cpp

HEADERS += launcherwindow.h \
    actionselectdialog.h

FORMS += launcherwindow.ui \
    actionselectdialog.ui

DISTFILES += \
    inputcontainer.qml
