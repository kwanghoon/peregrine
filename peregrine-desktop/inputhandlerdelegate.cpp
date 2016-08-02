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
    // #TODO: 단순히, 현재 액션의 링크와 inputText가 매치하는 것을
    // 서제스천 박스에 세팅하게 해보자.
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
