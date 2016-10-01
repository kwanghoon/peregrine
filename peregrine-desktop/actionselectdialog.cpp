#include "actionselectdialog.h"
#include "ui_actionselectdialog.h"
#include "actionuihelper.h"
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QPainter>

using namespace std;

ActionSelectDialog::ActionSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionSelectDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->frame->setStyleSheet(
        ".QFrame{ background-color: LightGreen; border: 1px solid Green; border-radius: 60px; }");
    selectedActionImage_ = new QLabel(this);
    {
        selectedActionImage_->resize(kActionImageSize);
        selectedActionImage_->setAlignment(Qt::AlignCenter);
    }
}

ActionSelectDialog::~ActionSelectDialog()
{
    delete ui;
}

void ActionSelectDialog::moveForSelectionDisplay(QPoint pos)
{
    QPoint p = selectedActionImage_->mapTo(this, QPoint());
    this->move(pos - p);
}

void ActionSelectDialog::setActionAssignInfo(const std::vector<ActionAssignInfo>& assignInfo)
{
    // clear
    for (auto& row : slotMap_)
    {
        for (auto& e : row)
        {
            delete e.imageLabel;
        }
    }
    slotMap_ = {};

    selectionPosUpperLimit_ = { 1, 1 };
    selectionPosLowerLimit_ = { -1, -1 };
    for (auto& e : assignInfo)
    {
        auto& slot = getActionSlotByPos(e.pos);
        slot.actionId = e.id;

        selectionPosLowerLimit_.rx() = min(selectionPosLowerLimit_.x(), e.pos.x());
        selectionPosLowerLimit_.ry() = min(selectionPosLowerLimit_.y(), e.pos.y());
        selectionPosUpperLimit_.rx() = max(selectionPosUpperLimit_.x(), e.pos.x());
        selectionPosUpperLimit_.ry() = max(selectionPosUpperLimit_.y(), e.pos.y());

        QLabel* label = new QLabel(this);
        ActionUIHelper::loadActionImage(label, e.imagePath, e.id);
        slot.imageLabel = label;
    }
    selectionPosLowerLimit_.rx() = min(selectionPosLowerLimit_.rx(), 0);
    selectionPosLowerLimit_.ry() = min(selectionPosLowerLimit_.ry(), 0);
    updateLayout();
}

QString ActionSelectDialog::getSelectedActionId() const
{
    return getActionSlotByPos(selectedPos_).actionId;
}

void ActionSelectDialog::showEvent(QShowEvent*)
{
    selectedPos_ = { 0, 0 };
}

void ActionSelectDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key::Key_Right || 
        event->key() == Qt::Key::Key_Left ||
        event->key() == Qt::Key::Key_Down ||
        event->key() == Qt::Key::Key_Up)
    {
        handleArrowKeyPressed(event->key());
    }
}

void ActionSelectDialog::keyReleaseEvent(QKeyEvent* event)
{
    bool pushDown = false;
    if (event->key() == Qt::Key::Key_Control || event->key() == Qt::Key::Key_Alt)
    {
        pushDown = true;
    }
    else if (event->key() == Qt::Key::Key_Escape)
    {
        pushDown = true;
    }
    if (pushDown)
    {
        this->hide();
    }
}

void ActionSelectDialog::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawRect(
        origin.x() + kActionImageSize.width() * selectedPos_.x() + kGapHori * selectedPos_.x(),
        origin.y() + kActionImageSize.height() * selectedPos_.y() + kGapVert * selectedPos_.y(),
        kActionImageSize.width(), kActionImageSize.height());
}

void ActionSelectDialog::handleArrowKeyPressed(int key)
{
    if (key == Qt::Key::Key_Right)
    {
        selectedPos_.rx() += 1;
        if (selectedPos_.x() > selectionPosUpperLimit_.x())
        {
            selectedPos_.rx() = selectionPosLowerLimit_.x();
        }
    }
    else if (key == Qt::Key::Key_Left)
    {
        selectedPos_.rx() -= 1;
        if (selectedPos_.x() < selectionPosLowerLimit_.x())
        {
            selectedPos_.rx() = selectionPosUpperLimit_.x();
        }
    }
    else if (key == Qt::Key::Key_Down)
    {
        selectedPos_.ry() += 1;
        if (selectedPos_.y() > selectionPosUpperLimit_.y())
        {
            selectedPos_.ry() = selectionPosLowerLimit_.y();
        }
    }
    else if (key == Qt::Key::Key_Up)
    {
        selectedPos_.ry() -= 1;
        if (selectedPos_.y() < selectionPosLowerLimit_.y())
        {
            selectedPos_.ry() = selectionPosUpperLimit_.y();
        }
    }
    if (selectedPos_ != QPoint(0, 0))
    {
        auto& selectedSlot = getActionSlotByPos(selectedPos_);
        if (!selectedSlot.actionId.isEmpty())
        {
            auto* label = selectedSlot.imageLabel;
            if (label->pixmap())
            {
                auto pixmap = *label->pixmap();
                selectedActionImage_->setPixmap(std::move(pixmap));
            }
            else
            {
                selectedActionImage_->setText(label->text());
            }
        }
    }
    this->update();
}

ActionSelectDialog::Slot& ActionSelectDialog::getActionSlotByPos(const QPoint& pos)
{
    return slotMap_[5 + pos.y()][5 + pos.x()];
}

const ActionSelectDialog::Slot& ActionSelectDialog::getActionSlotByPos(const QPoint& pos) const
{
    return slotMap_[5 + pos.y()][5 + pos.x()];
}

void ActionSelectDialog::updateLayout()
{
    // set dialog size to be able to contain all action slots.
    int horiCount = selectionPosUpperLimit_.rx() - selectionPosLowerLimit_.rx() + 1;
    int vertiCount = selectionPosUpperLimit_.ry() - selectionPosLowerLimit_.ry() + 1;
    ui->frame->resize((kActionImageSize.width() + kGapHori) * horiCount + 40,
        (kActionImageSize.height() + kGapVert) * vertiCount + 40);
    this->resize(ui->frame->width() + 20, ui->frame->height() + 20);
    ui->frame->move(10, 10);

    // set position
    origin = { (kActionImageSize.width() + kGapHori) * -selectionPosLowerLimit_.rx() + 20,
        (kActionImageSize.height() + kGapVert) * -selectionPosLowerLimit_.ry() + 20 };
    selectedActionImage_->move(origin.x(), origin.y());    
    for (int y = -5; y <= 4; y++)
    {
        for (int x = -5; x <= 4; x++)
        {
            auto& slot = getActionSlotByPos({ x, y });
            if (slot.actionId.isEmpty())
            {
                continue;
            }
            slot.imageLabel->move(origin.x() + kActionImageSize.width() * x + kGapHori * x,
                origin.y() + kActionImageSize.height() * y + kGapVert * y);
        }
    }
}