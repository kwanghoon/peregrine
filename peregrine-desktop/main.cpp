#include "launcherwindow.h"
#include <QApplication>
#include <QtWebView/QtWebView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LauncherWindow w;
    QtWebView::initialize();
    w.show();

    return a.exec();
}
