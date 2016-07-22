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
    ui(new Ui::ActionSelectDialog)
{
    ui->setupUi(this);
    loadActionImages();
}

ActionSelectDialog::~ActionSelectDialog()
{
    delete ui;
}

void ActionSelectDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Right)
    {
        qDebug() << "right key pressed on select dlg";
    }
}

void ActionSelectDialog::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Shift)
    {
        this->hide();
    }
}

void ActionSelectDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const QSize kActionImageSize{ 120, 60 };
    const QPoint kOrigin{
        this->size().width() / 2 - kActionImageSize.width() / 2,
        this->size().height() / 2 - kActionImageSize.height() / 2 };
    painter.drawRect(kOrigin.x(), kOrigin.y(), kActionImageSize.width(), kActionImageSize.height());
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
    const QSize kActionImageSize{ 120, 60 };
    const QPoint kOrigin{ 
        this->size().width() / 2 - kActionImageSize.width() / 2, 
        this->size().height() / 2 - kActionImageSize.height() / 2};
    const int kGapHori = 10;
    const int kGapVert = 10;

    for (auto& p : placementData)
    {
        createImage(this,
            kOrigin.x() + kActionImageSize.width() * p.pos.x() + kGapHori * p.pos.x(),
            kOrigin.y() + kActionImageSize.height() * p.pos.y() + kGapVert * p.pos.y(),
            p.name);
    }
}
