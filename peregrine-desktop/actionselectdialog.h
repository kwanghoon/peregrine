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
    virtual void showEvent(QShowEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    void loadActionImages();

    const QSize kActionImageSize{ 120, 60 };
    const QPoint kOrigin;
    const int kGapHori = 10;
    const int kGapVert = 10;
    Ui::ActionSelectDialog *ui;
    QPoint selectedPos_;
    QPoint selectionPosUpperLimit_;
    QPoint selectionPosLowerLimit_;
};

#endif // ACTIONSELECTDIALOG_H
