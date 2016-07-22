#ifndef ACTIONSELECTDIALOG_H
#define ACTIONSELECTDIALOG_H

#include <QDialog>

namespace Ui {
class ActionSelectDialog;
}

class ActionSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ActionSelectDialog(QWidget *parent = 0);
    ~ActionSelectDialog();

private:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

    Ui::ActionSelectDialog *ui;
};

#endif // ACTIONSELECTDIALOG_H
