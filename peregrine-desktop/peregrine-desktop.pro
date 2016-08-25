
QT += core gui \
    xml \
    quickwidgets \
    webview \
    network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG -= flat
CONFIG += c++11

TARGET = peregrine-desktop
TEMPLATE = app

INCLUDEPATH += ../plugin-sdk \
    ../thirdparty \
    ..

SOURCES += main.cpp \
    action.cpp \
    plugin.cpp \
    global.cpp \
    launcherwindow.cpp \
    actionselectdialog.cpp \
    inputhandlerdelegate.cpp \
    suggestionlistcontroller.cpp \
    actionuihelper.cpp \
    configurationwindow.cpp \
    syncmanager.cpp \
    ../utils/StartupRegister.cpp \
    configmanager.cpp

HEADERS += action.h \
    plugin.h \
    global.h \
    userconfig.h \
    launcherwindow.h \
    actionselectdialog.h \
    inputhandlerdelegate.h \
    ../plugin-sdk/peregrine-plugin-sdk.h \
    suggestionlistcontroller.h \
    actionuihelper.h \
    configurationwindow.h \
    syncmanager.h \
    ../utils/StartupRegister.h \
    configmanager.h

FORMS += launcherwindow.ui \
    actionselectdialog.ui \
    configurationwindow.ui

DISTFILES += \
    inputcontainer.qml \
    SuggestionListView.qml \
    ConfigurationForm.ui.qml

# boost
INCLUDEPATH	+= $$(BOOST_ROOT)

# Jinja2CppLight
INCLUDEPATH += ../thirdparty/Jinja2CppLight/src
SOURCES += $$files(../thirdparty/Jinja2CppLight/src/*.cpp)
HEADERS += $$files(../thirdparty/Jinja2CppLight/src/*.h)

win32-msvc* {
    QMAKE_CXXFLAGS += /bigobj
}

equals(TEMPLATE, "vcapp") {
    CONFIG(release, debug|release):OPTIM_MODE=release
    CONFIG(debug, debug|release):OPTIM_MODE=debug

    QMAKE_POST_LINK += \
        call $$quote($$shell_path($$_PRO_FILE_PWD_/../scripts/copy-outputs.bat)) $$OPTIM_MODE $$quote($$shell_path($$_PRO_FILE_PWD_/..)) $$escape_expand(\\n)
}
