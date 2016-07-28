#include "actionselectdialog.h"
#include "ui_actionselectdialog.h"
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QImageReader>
#include <QPainter>

using namespace std;

namespace
{
    QLabel* createImage(QWidget* parent, int x, int y, const QString& path)
    {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        auto pixmap = QPixmap::fromImage(reader.read());

        const QSize kActionImageSize{120, 60};
        QLabel* image = new QLabel(parent);
        image->move(x, y);
        image->resize(kActionImageSize);
        image->setAlignment(Qt::AlignCenter);
        image->setPixmap(pixmap.scaled(kActionImageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        return image;
    }
}

ActionSelectDialog::ActionSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionSelectDialog),
    kOrigin{this->size().width() / 2 - kActionImageSize.width() / 2, this->size().height() / 2 - kActionImageSize.height() / 2 }
{
    selectionPosUpperLimit_ = { 1, 1 };
    selectionPosLowerLimit_ = { -1, -1 };
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    selectedActionImage_ = new QLabel(this);
    {
        selectedActionImage_->move(kOrigin.x(), kOrigin.y());
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
    this->move(pos - kOrigin);
}

void ActionSelectDialog::setActionAssignInfo(const std::vector<ActionAssignInfo>& assignInfo)
{
    for (auto& e : assignInfo)
    {
        slotMap_[5 + e.pos.y()][5 + e.pos.x()] = e.id;
        data_[e.id].actionId = e.id;

        selectionPosLowerLimit_.rx() = min(selectionPosLowerLimit_.x(), e.pos.x());
        selectionPosLowerLimit_.ry() = min(selectionPosLowerLimit_.y(), e.pos.y());
        selectionPosUpperLimit_.rx() = max(selectionPosUpperLimit_.x(), e.pos.x());
        selectionPosUpperLimit_.ry() = max(selectionPosUpperLimit_.y(), e.pos.y());

        qDebug() << e.imagePath;
        QLabel* label = nullptr;
        if (e.imagePath.isEmpty())
        {
            label = new QLabel(e.id, this);
            label->move(kOrigin.x() + kActionImageSize.width() * e.pos.x() + kGapHori * e.pos.x(),
                kOrigin.y() + kActionImageSize.height() * e.pos.y() + kGapVert * e.pos.y());
            label->resize(kActionImageSize);
            label->setAlignment(Qt::AlignCenter);
        }
        else
        {
            label = createImage(this,
                kOrigin.x() + kActionImageSize.width() * e.pos.x() + kGapHori * e.pos.x(),
                kOrigin.y() + kActionImageSize.height() * e.pos.y() + kGapVert * e.pos.y(),
                e.imagePath);
        }
        data_[e.id].imageLabel = label;
    }
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
    if (event->key() == Qt::Key::Key_Shift ||
        event->key() == Qt::Key::Key_Escape)
    {
        this->hide();
    }
}

void ActionSelectDialog::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawRect(
        kOrigin.x() + kActionImageSize.width() * selectedPos_.x() + kGapHori * selectedPos_.x(),
        kOrigin.y() + kActionImageSize.height() * selectedPos_.y() + kGapVert * selectedPos_.y(),
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
    QString selectedId;
    if (selectedPos_ != QPoint(0, 0))
    {
        selectedId = slotMap_[5 + selectedPos_.y()][5 + selectedPos_.x()];
        if (!selectedId.isEmpty())
        {
            auto* label = data_[selectedId].imageLabel;
            if (label->pixmap())
            {
                auto pixmap = *data_[selectedId].imageLabel->pixmap();
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