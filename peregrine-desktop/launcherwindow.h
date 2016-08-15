#ifndef LAUNCHERWINDOW_H
#define LAUNCHERWINDOW_H

#include "actionselectdialog.h"
#include <QMainWindow>
#include <memory>

namespace Ui {
    class LauncherWindow;
}
class InputHandlerDelegate;
class SuggestionListController;
class QSystemTrayIcon;
class SyncManager;

class LauncherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LauncherWindow(QWidget *parent = 0);
    ~LauncherWindow();

private:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

    void initSuggestionListController();
    void loadSetting();
    void loadPlugins();
    void tryLoadPlugin(QString path);
    void showActionSelectDialog();
    void changeAction(QString actionId);
    void setupTrayIcon();
    void popUp();
    void pushDown();

    // QML widget input listeners
    void onInputTextChanged(const QString& inputText);
    void onKeyPressed(int key, int modifiers, QString inputText);

    // history control
    void initHistory();
    void saveHistory();
    void switchToPreviousAction();
    void switchToNextAction();

    Ui::LauncherWindow *ui;
    ActionSelectDialog actionSelectDlg_;
    InputHandlerDelegate* inputHandlerDelegate_;
    QString currentAction_;
    QSystemTrayIcon* tray_ = nullptr;
    std::unique_ptr<SyncManager> syncManager_;
    std::list<QString> actionHistory_;
    decltype(actionHistory_)::iterator actionHistoryPointer_;
};

#endif // LAUNCHERWINDOW_H
