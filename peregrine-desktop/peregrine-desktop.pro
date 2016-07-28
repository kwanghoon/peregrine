QT += core gui \
    xml \
    quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = peregrine-desktop
TEMPLATE = app

INCLUDEPATH += ../plugin-sdk

SOURCES += main.cpp \
    action.cpp \
    plugin.cpp \
    launcherwindow.cpp \
    actionselectdialog.cpp \
    inputhandlerdelegate.cpp

HEADERS += action.h \
    plugin.h \
    launcherwindow.h \
    actionselectdialog.h \
    inputhandlerdelegate.h \
    ../plugin-sdk/peregrine-plugin-sdk.h

FORMS += launcherwindow.ui \
    actionselectdialog.ui

DISTFILES += \
    inputcontainer.qml
