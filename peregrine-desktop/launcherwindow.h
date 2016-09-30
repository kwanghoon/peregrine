#ifndef LAUNCHERWINDOW_H
#define LAUNCHERWINDOW_H

#include "actionselectdialog.h"
#include <QMainWindow>
#include <QTime>

namespace Ui {
    class LauncherWindow;
}
class InputHandlerDelegate;
class SuggestionListController;
class QSystemTrayIcon;
class SyncManager;
class Action;

class LauncherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LauncherWindow(QWidget *parent = 0);
    ~LauncherWindow();

    void onConfigUpdated();

private:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void moveEvent(QMoveEvent *event) override;

    void initializeUI();
    void initSuggestionListController();
    void updateSuggestionListPosition();
    void loadPlugins();
    void tryLoadPlugin(QString path);
    void showActionSelectDialog();
    void changeAction(QString actionId, QString inputText = QString());
    void setupTrayIcon();
    void popUp();
    void pushDown();
    void suggestLinkedActions(Action* currAction, Action* adoptedAction, const QString& input);

    void setHeaderText(const QString& text);

    // QWidget 
    virtual void closeEvent(QCloseEvent *event) override;
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    // QML widget input listeners
    void onInputTextChanged(const QString& inputText);
    bool onKeyPressed(int key, int modifiers, const QString& inputText);

    // history control
    void initHistory();
    void saveHistory();
    void switchToPreviousAction();
    void switchToNextAction();
    void saveInputHistory(const QString& inputText);
    void recallInputHistory();

    Ui::LauncherWindow *ui;
    ActionSelectDialog actionSelectDlg_;
    class QQuickWidget* suggestionBox_ = nullptr;
    InputHandlerDelegate* inputHandlerDelegate_;
    QString currentAction_;
    std::list<QString> actionHistory_;
    decltype(actionHistory_)::iterator actionHistoryPointer_;
    std::list<QString> inputHistory_;
    std::list<QString>::const_iterator inputHistoryIterator_;
    QTime lastInputHistoryUnrolledTimer_;

    bool appExit_ = false;
    bool holdingWindow_ = false;
    QPoint mousePressPos_;

    QSystemTrayIcon* tray_ = nullptr;
    QAction* toggleShortcutAction_ = nullptr;
    QAction* showAction_ = nullptr;
    QAction* hideAction_ = nullptr;

#   ifdef Q_OS_WIN
    const int kHotKey_PopUpId_ = 123;
#   ifndef NDEBUG
    const int kHotKey_ExitId_ = 124;
    const int kHotKey_ConfigId_ = 125;
#   endif
#   endif // Q_OS_WIN
};

#endif // LAUNCHERWINDOW_H
