#include "launcherwindow.h"
#include <QApplication>
#include <QtWebView/QtWebView>
#include <QSharedMemory>
#include <QDebug>
#include <QDir>

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

    QDir::setCurrent(QApplication::applicationDirPath());

    LauncherWindow w;
    QtWebView::initialize();
    w.show();

    return a.exec();
}
