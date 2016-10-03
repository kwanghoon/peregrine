#include "actionselectdialog.h"
#include "ui_actionselectdialog.h"
#include "actionuihelper.h"
#include "action.h"
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QImageReader>

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
    initActionCursorUI();
}

void ActionSelectDialog::initActionCursorUI()
{
    selectedActionImage_ = new QLabel(this);
    {
        selectedActionImage_->resize(kActionImageSize);
        selectedActionImage_->setAlignment(Qt::AlignCenter);
    }
    selectedActionCursor_ = new QLabel(this);
    {
        const QSize kSize{ 80, 45 };

        QImageReader reader("peregrine.svg");
        reader.setAutoTransform(true);
        auto pixmap = QPixmap::fromImage(reader.read());

        selectedActionCursor_->resize(kSize);
        selectedActionCursor_->setAlignment(Qt::AlignCenter);
        selectedActionCursor_->setPixmap(pixmap.scaled(kSize,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
    selectedActionCursorBackground_ = new QFrame(this);
    {
        selectedActionCursorBackground_->resize(kActionImageSize);
        selectedActionCursorBackground_->setStyleSheet(
            ".QFrame{ background-color: LightSeaGreen; border-radius: 10px; }");
    }
    selectedActionNameBackground_ = new QFrame(this);
    {
        selectedActionNameBackground_->setStyleSheet(
            ".QFrame{ background-color: LightSeaGreen; border-radius: 10px; }");
    }
    selectedActionName_ = new QLabel(this);
    {
        QFont font("Optima", 14, QFont::Bold);
        selectedActionName_->setFont(font);
        selectedActionName_->setStyleSheet("QLabel { color: Indigo }");
        selectedActionName_->setAlignment(Qt::AlignHCenter);
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
    moveCursor();
    selectedActionCursor_->raise();
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
        else
        {
            selectedActionImage_->clear();
        }
    }
    else
    {
        selectedActionImage_->clear();
    }
    moveCursor();
}

void ActionSelectDialog::moveCursor()
{
    // move background panel
    int x = origin.x() + (kActionImageSize.width() + kGapHori) * selectedPos_.x();
    int y = origin.y() + (kActionImageSize.height() + kGapVert) * selectedPos_.y();
    selectedActionCursorBackground_->move(x, y);

    // move cursor
    int centerX = x + kActionImageSize.width() / 2;
    int centerY = y + kActionImageSize.height() / 2;

    int xOffset = 0;
    auto& slot = getActionSlotByPos(selectedPos_);
    if (slot.imageLabel && slot.imageLabel->pixmap() != nullptr)
    {
        xOffset = -(slot.imageLabel->width() - slot.imageLabel->pixmap()->width()) / 2;
    }
    selectedActionCursor_->move(centerX + 15 + xOffset, centerY - 50);

    // display name of the selected action
    if (!slot.actionId.isEmpty())
    {
        selectedActionName_->setText(
            ActionManager::getInstance().getActionById(getSelectedActionId())->name);
    }
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
        (kActionImageSize.height() + kGapVert) * vertiCount + 60);
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

    selectedActionName_->setFixedWidth(250);
    selectedActionName_->move(origin.x() + kActionImageSize.width() / 2 - selectedActionName_->width() / 2,
        ui->frame->height() - 25);

    selectedActionNameBackground_->resize(selectedActionName_->width(), 30);
    selectedActionNameBackground_->move(selectedActionName_->x(), selectedActionName_->y() - 2);
}