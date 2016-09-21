#include "launcherwindow.h"
#include "global.h"
#include <QApplication>
#include <QSharedMemory>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QtWebView/QtWebView>
#include <algorithm>

namespace
{
    std::unique_ptr<QSharedMemory> sharedObject;
}

int main(int argc, char *argv[])
{
    sharedObject.reset(new QSharedMemory("--==Peregrine==--"));
    if (!sharedObject->create(4, QSharedMemory::AccessMode::ReadWrite))
    {
        qDebug() << "Peregrine is already running";
        return 1;
    }

    QApplication a(argc, argv);

    if (argc > 1)
    {
        auto isVersionArgument = [](const char* arg) {
            return (strcmp(arg, "--version") == 0 || stricmp(arg, "-v") == 0);
        };
        if (std::any_of(argv + 1, argv + argc, isVersionArgument))
        {
            QString s = QString("Peregrine Version: %1").arg(global::getAppVersion());

            QMessageBox msgBox;
            msgBox.setText(s);
            msgBox.exec();

            qDebug() << s;
            return 0;
        }
    }

    QDir::setCurrent(QApplication::applicationDirPath());

    LauncherWindow w;
    QtWebView::initialize();
    w.show();

    return a.exec();
}
