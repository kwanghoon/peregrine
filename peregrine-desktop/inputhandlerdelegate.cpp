#include "inputhandlerdelegate.h"
#include "action.h"
#include "plugin.h"
#include <QLibrary>
#include <QDebug>

InputHandlerDelegate::InputHandlerDelegate(QObject *parent) : QObject(parent)
{}

void InputHandlerDelegate::onInputTextAccepted(const QString& inputText)
{
    qDebug() << inputText;
    auto action = ActionManager::getInstance().getActionById(currentAction);
    auto plugin = action->plugin;
    if (plugin)
    {
        plugin->runAction(currentAction, inputText);
    }
    else
    {
        // #TODO
    }
}
