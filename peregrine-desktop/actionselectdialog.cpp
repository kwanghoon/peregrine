#include "actionselectdialog.h"
#include "ui_actionselectdialog.h"
#include <QKeyEvent>
#include <QDebug>

ActionSelectDialog::ActionSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionSelectDialog)
{
    ui->setupUi(this);
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
