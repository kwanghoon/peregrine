#ifndef ACTIONSELECTDIALOG_H
#define ACTIONSELECTDIALOG_H

#include <QDialog>
#include <array>

namespace Ui 
{
    class ActionSelectDialog;
}
class QLabel;

class ActionSelectDialog : public QDialog
{
    Q_OBJECT

public:
    struct ActionAssignInfo
    {
        QString id;
        QPoint pos;
        QString imagePath;
    };
    explicit ActionSelectDialog(QWidget *parent = 0);
    ~ActionSelectDialog();
    void moveForSelectionDisplay(QPoint pos);
    void setActionAssignInfo(const std::vector<ActionAssignInfo>& assignInfo);
    QString getSelectedActionId() const;

private:
    struct Slot
    {
        QString actionId;
        QLabel* imageLabel;
    };
    virtual void showEvent(QShowEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    void handleArrowKeyPressed(int key);
    QString& getActionIdByPos(const QPoint& pos);
    const QString& getActionIdByPos(const QPoint& pos) const;

    const QSize kActionImageSize{ 120, 60 };
    const QPoint kOrigin;
    const int kGapHori = 10;
    const int kGapVert = 10;
    Ui::ActionSelectDialog *ui;
    QLabel* selectedActionImage_;
    QPoint selectedPos_;
    QPoint selectionPosUpperLimit_;
    QPoint selectionPosLowerLimit_;
    std::array<std::array<QString, 10>, 10> slotMap_; // in row-major order
    std::map<QString, Slot> data_;
};

#endif // ACTIONSELECTDIALOG_H