#ifndef LAUNCHERWINDOW_H
#define LAUNCHERWINDOW_H

#include <QMainWindow>

namespace Ui {
class LauncherWindow;
}

class LauncherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LauncherWindow(QWidget *parent = 0);
    ~LauncherWindow();

private:
    Ui::LauncherWindow *ui;
};

#endif // LAUNCHERWINDOW_H
