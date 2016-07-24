#-------------------------------------------------
#
# Project created by QtCreator 2016-07-21T13:25:22
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = peregrine-desktop
TEMPLATE = app


SOURCES += main.cpp\
        launcherwindow.cpp \
    actionselectdialog.cpp

HEADERS  += launcherwindow.h \
    actionselectdialog.h

FORMS    += launcherwindow.ui \
    actionselectdialog.ui
