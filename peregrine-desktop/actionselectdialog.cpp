#include "actionselectdialog.h"
#include "ui_actionselectdialog.h"
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>
#include <QImageReader>
#include <QPainter>

namespace
{
    void createImage(QWidget* parent, int x, int y, const char *path)
    {
        QImageReader reader(path);
        reader.setAutoTransform(true);
        auto pixmap = QPixmap::fromImage(reader.read());

        const QSize kActionImageSize{120, 60};
        QLabel* image = new QLabel(parent);
        image->move(x, y);
        image->resize(kActionImageSize);
        image->setPixmap(pixmap.scaled(kActionImageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        image->setAlignment(Qt::AlignCenter);
    }
}

ActionSelectDialog::ActionSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionSelectDialog),
    kOrigin{this->size().width() / 2 - kActionImageSize.width() / 2, this->size().height() / 2 - kActionImageSize.height() / 2 }
{
    ui->setupUi(this);
    selectionPosUpperLimit_ = { 1, 1 };
    selectionPosLowerLimit_ = { -1, -1 };
    loadActionImages();
}

ActionSelectDialog::~ActionSelectDialog()
{
    delete ui;
}

void ActionSelectDialog::showEvent(QShowEvent *event)
{
    selectedPos_ = { 0, 0 };
}

void ActionSelectDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Right) 
    { 
        selectedPos_.rx() += 1; 
        if (selectedPos_.x() > selectionPosUpperLimit_.x())
        {
            selectedPos_.rx() = selectionPosLowerLimit_.x();
        }
        this->update();
    }
    else if (event->key() == Qt::Key::Key_Left) 
    {
        selectedPos_.rx() -= 1;
        if (selectedPos_.x() < selectionPosLowerLimit_.x())
        {
            selectedPos_.rx() = selectionPosUpperLimit_.x();
        }
        this->update();
    }
    else if (event->key() == Qt::Key::Key_Down) 
    {
        selectedPos_.ry() += 1;
        if (selectedPos_.y() > selectionPosUpperLimit_.y())
        {
            selectedPos_.ry() = selectionPosLowerLimit_.y();
        }
        this->update();
    }
    else if (event->key() == Qt::Key::Key_Up) 
    {
        selectedPos_.ry() -= 1;
        if (selectedPos_.y() < selectionPosLowerLimit_.y())
        {
            selectedPos_.ry() = selectionPosUpperLimit_.y();
        }
        this->update();
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
        createImage(this,
            kOrigin.x() + kActionImageSize.width() * p.pos.x() + kGapHori * p.pos.x(),
            kOrigin.y() + kActionImageSize.height() * p.pos.y() + kGapVert * p.pos.y(),
            p.name);
    }
}
