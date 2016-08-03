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

class LauncherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LauncherWindow(QWidget *parent = 0);
    ~LauncherWindow();

private:
    virtual void keyPressEvent(QKeyEvent *event) override;
    void initSuggestionListController();
    void loadSetting();
    void loadPlugins();
    void tryLoadPlugin(QString path);
    void changeAction(QString actionId);

    // QML widget input listeners
    void onInputTextAccepted(const QString& inputText);
    void onInputTextChanged(const QString& inputText);
    void onSuggestionItemClicked(int index);

    Ui::LauncherWindow *ui;
    ActionSelectDialog actionSelectDlg_;
    InputHandlerDelegate* inputHandlerDelegate_;
    QString currentAction_;
};

#endif // LAUNCHERWINDOW_H
