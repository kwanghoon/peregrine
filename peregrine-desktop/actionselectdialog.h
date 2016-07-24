#ifndef ACTIONSELECTDIALOG_H
#define ACTIONSELECTDIALOG_H

#include <QDialog>
#include <array>

namespace Ui {
class ActionSelectDialog;
}
class QLabel;

class ActionSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ActionSelectDialog(QWidget *parent = 0);
    ~ActionSelectDialog();
    void moveForSelectionDisplay(QPoint pos);

private:
    virtual void showEvent(QShowEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    void loadActionImages();
    void handleArrowKeyPressed(int key);

    const QSize kActionImageSize{ 120, 60 };
    const QPoint kOrigin;
    const int kGapHori = 10;
    const int kGapVert = 10;
    Ui::ActionSelectDialog *ui;
    QPoint selectedPos_;
    QPoint selectionPosUpperLimit_;
    QPoint selectionPosLowerLimit_;
    std::array<std::array<QLabel*, 10>, 10> actionImageMap_; // in row-major order
    void loadSetting();
};

#endif // ACTIONSELECTDIALOG_H
