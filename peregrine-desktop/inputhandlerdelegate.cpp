#include "inputhandlerdelegate.h"
#include <QDebug>

InputHandlerDelegate::InputHandlerDelegate(QObject *parent) : QObject(parent)
{}

void InputHandlerDelegate::onInputTextAccepted(const QString& inputText)
{
    qDebug() << inputText;
}
