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

        const QSize kActionImageSize{120, 80};
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

    // 액션 이미지
    createImage(this, 10, 10, "google.png");
    createImage(this, 10, 150, "man.jpg");
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
    painter.drawRect(10, 10, 120, 80);
    painter.drawRect(10, 150, 120, 80);
}