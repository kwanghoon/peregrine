#include "launcherwindow.h"
#include <QApplication>
#include <QtWebView/QtWebView>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir::setCurrent(QApplication::applicationDirPath());

    LauncherWindow w;
    QtWebView::initialize();
    w.show();

    return a.exec();
}
