#include "inputhandlerdelegate.h"
#include "action.h"
#include "plugin.h"
#include "global.h"
#include "suggestionlistcontroller.h"
#include <QLibrary>
#include <QDebug>

InputHandlerDelegate::InputHandlerDelegate(QObject *parent) 
    : QObject(parent)
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

Q_INVOKABLE void InputHandlerDelegate::onInputTextChanged(const QString& inputText)
{
    // #TODO: �ܼ���, ���� �׼��� ��ũ�� inputText�� ��ġ�ϴ� ����
    // ������õ �ڽ��� �����ϰ� �غ���.
    Action* currAction = ActionManager::getInstance().getActionById(currentAction);
    global::suggestionListController->clear();
    if (inputText.isEmpty())
    {
        return;
    }
    for (auto& l : currAction->links)
    {
        if (l.keyword.startsWith(inputText))
        {
            Action* linkedAction = ActionManager::getInstance().getActionById(l.linkedActionId);
            if (!linkedAction)
            {
                continue;
            }
            
            QString s = "Move to '" + linkedAction->name + "' Action";
            global::suggestionListController->add(s);
        }
    }
}
