// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#include "launcherwindow.h"
#include "global.h"
#include <QApplication>
#include <QSharedMemory>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#ifdef WEBVIEW_SUPPORT
#   include <QtWebView/QtWebView>
#endif
#include <algorithm>
#include <memory>

namespace
{
    std::unique_ptr<QSharedMemory> sharedObject;
}

int main(int argc, char *argv[])
{
    // prevent duplicated execution
    sharedObject.reset(new QSharedMemory("--==Peregrine==--"));
    if (!sharedObject->create(4, QSharedMemory::AccessMode::ReadWrite))
    {
        qDebug() << "Peregrine is already running";
        return 1;
    }

    QApplication a(argc, argv);

    // if '--version' argument is specified, show the version information and exit program.
    if (argc > 1)
    {
        auto isVersionArgument = [](const char* arg) {
            return (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0);
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
#   if defined(WEBVIEW_SUPPORT)
    QtWebView::initialize();
#   endif
    w.show();

    return a.exec();
}
