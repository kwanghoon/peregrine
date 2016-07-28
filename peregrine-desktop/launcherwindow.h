#ifndef LAUNCHERWINDOW_H
#define LAUNCHERWINDOW_H

#include "actionselectdialog.h"
#include <QMainWindow>

namespace Ui {
    class LauncherWindow;
}
class InputHandlerDelegate;

class LauncherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LauncherWindow(QWidget *parent = 0);
    ~LauncherWindow();

private:
    virtual void keyPressEvent(QKeyEvent *event) override;
    void loadSetting();
    void loadPlugins();
    void tryLoadPlugin(QString path);

    Ui::LauncherWindow *ui;
    ActionSelectDialog actionSelectDlg_;
    InputHandlerDelegate* inputHandlerDelegate_;
};

#endif // LAUNCHERWINDOW_H
