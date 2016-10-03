
QT += core gui \
    xml \
    quickwidgets \
    network

!win32-g++ {
    QT += webview
    DEFINES += WEBVIEW_SUPPORT
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG -= flat
CONFIG += c++11

TARGET = peregrine-desktop
TEMPLATE = app

CONFIG(release, debug|release):OPTIM_MODE=release
CONFIG(debug, debug|release):OPTIM_MODE=debug
CONFIG(debug, debug|release):OPTIM_SUFFIX=d

RC_ICONS = appicon.ico

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
    ConfigurationForm.ui.qml \
    CustomUi.qml \
    ActionSlotConfig.qml \
    AccountConfig.qml \
    GeneralConfig.qml

# boost
INCLUDEPATH += $$(BOOST_ROOT)

# Jinja2CppLight
INCLUDEPATH += ../thirdparty/Jinja2CppLight/src
SOURCES += $$files(../thirdparty/Jinja2CppLight/src/*.cpp)
HEADERS += $$files(../thirdparty/Jinja2CppLight/src/*.h)

win32-msvc* {
    QMAKE_CXXFLAGS += /bigobj
}

equals(TEMPLATE, "vcapp") {
    QMAKE_POST_LINK += \
        call $$quote($$shell_path($$_PRO_FILE_PWD_/../scripts/copy-outputs.bat)) $$OPTIM_MODE $$quote($$shell_path($$_PRO_FILE_PWD_/..)) $$escape_expand(\\n)
}

# quazip
DEFINES += QUAZIP_STATIC
QUAZIP_DIR = ../thirdparty/quazip/quazip
include($$QUAZIP_DIR/quazip.pri)

# zlib for quazip
INCLUDEPATH += ../thirdparty/zlib \
    ../thirdparty/zlib-build
win32-msvc* {
    LIBS += -l$$absolute_path(../thirdparty/zlib-build/$$OPTIM_MODE/zlibstatic$$OPTIM_SUFFIX)
} else {
    LIBS += -L$$absolute_path(../thirdparty/zlib-build) -lz
}

# SimpleCrypt
SOURCES += $$files(../thirdparty/SimpleCrypt/*.cpp)
