#include "actionselectdialog.h"
#include "ui_actionselectdialog.h"
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QImageReader>
#include <QPainter>

namespace
{
    QLabel* createImage(QWidget* parent, int x, int y, const char *path)
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
    for (auto& a : actionImageMap_)
    {
        std::fill(a.begin(), a.end(), nullptr);
    }
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    loadSetting();
    loadActionImages();
}

ActionSelectDialog::~ActionSelectDialog()
{
    delete ui;
}

void ActionSelectDialog::moveForSelectionDisplay(QPoint pos)
{
    this->move(pos - kOrigin);
}

void ActionSelectDialog::showEvent(QShowEvent *event)
{
    selectedPos_ = { 0, 0 };
}

void ActionSelectDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Right || 
        event->key() == Qt::Key::Key_Left ||
        event->key() == Qt::Key::Key_Down ||
        event->key() == Qt::Key::Key_Up)
    {
        handleArrowKeyPressed(event->key());
    }
}

void ActionSelectDialog::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Shift ||
        event->key() == Qt::Key::Key_Escape)
    {
        this->hide();
    }
}

void ActionSelectDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawRect(
        kOrigin.x() + kActionImageSize.width() * selectedPos_.x() + kGapHori * selectedPos_.x(),
        kOrigin.y() + kActionImageSize.height() * selectedPos_.y() + kGapVert * selectedPos_.y(),
        kActionImageSize.width(), kActionImageSize.height());
}

void ActionSelectDialog::loadActionImages()
{
    struct ImagePlacement
    {
        const char* name;
        QPoint pos;
    };
    std::vector<ImagePlacement> placementData{
        { "google.png", {1, 0} },
        { "man.jpg", {1, -1} },
        { "google.png",{-1, 0} },
    };

    for (auto& p : placementData)
    {
        auto* label = createImage(this,
            kOrigin.x() + kActionImageSize.width() * p.pos.x() + kGapHori * p.pos.x(),
            kOrigin.y() + kActionImageSize.height() * p.pos.y() + kGapVert * p.pos.y(),
            p.name);
        actionImageMap_[5 + p.pos.y()][5 + p.pos.x()] = label;
    }

    auto label = new QLabel(this);
    {
        label->move(kOrigin.x(), kOrigin.y());
        label->resize(kActionImageSize);
        label->setAlignment(Qt::AlignCenter);
    }
    actionImageMap_[5][5] = label;
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
    if (selectedPos_ != QPoint(0, 0) &&
        actionImageMap_[5 + selectedPos_.y()][5 + selectedPos_.x()] != nullptr)
    {
        actionImageMap_[5][5]->setPixmap(*actionImageMap_[5 + selectedPos_.y()][5 + selectedPos_.x()]->pixmap());
    }
    this->update();
}

void ActionSelectDialog::loadSetting()
{

}
