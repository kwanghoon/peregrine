// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#ifndef ACTIONSELECTDIALOG_H
#define ACTIONSELECTDIALOG_H

#include <QDialog>
#include <array>

namespace Ui 
{
    class ActionSelectDialog;
}
class QLabel;
class QFrame;

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
        QLabel* imageLabel = nullptr;
    };
    void initActionCursorUI();
    virtual void showEvent(QShowEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    void handleArrowKeyPressed(int key);

    Slot& getActionSlotByPos(const QPoint& pos);
    const Slot& getActionSlotByPos(const QPoint& pos) const;
    void updateLayout();
    void moveCursor();

    const QSize kActionImageSize{ 120, 60 };
    QPoint origin;
    const int kGapHori = 10;
    const int kGapVert = 10;
    Ui::ActionSelectDialog *ui;
    QLabel* selectedActionImage_;
    QLabel* selectedActionCursor_;
    QLabel* selectedActionName_;
    QFrame* selectedActionCursorBackground_;
    QFrame* selectedActionNameBackground_;
    QPoint selectedPos_;
    QPoint selectionPosUpperLimit_;
    QPoint selectionPosLowerLimit_;
    std::array<std::array<Slot, 10>, 10> slotMap_; // in row-major order
};

#endif // ACTIONSELECTDIALOG_H