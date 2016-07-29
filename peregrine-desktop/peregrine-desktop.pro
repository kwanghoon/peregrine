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

equals(TEMPLATE, "vcapp") {
    CONFIG(release, debug|release):OPTIM_MODE=release
    CONFIG(debug, debug|release):OPTIM_MODE=debug

    QMAKE_POST_LINK += \
        call $$quote($$shell_path($$_PRO_FILE_PWD_/../scripts/copy-outputs.bat)) $$OPTIM_MODE $$quote($$shell_path($$_PRO_FILE_PWD_/..)) $$escape_expand(\\n)
}