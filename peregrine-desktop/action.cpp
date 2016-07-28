#include "action.h"
#include "plugin.h"

using namespace std;

bool Action::run(const QString& input)
{
    if (plugin)
    {
        int ret = plugin->runAction(id, input);
        if (ret < 0)
        {
            return false;
        }
    }
    // #TODO
    return false;
}

bool ActionManager::addAction(std::unique_ptr<Action> action)
{
    // #TODO: check duplication
    actions_.push_back(move(action));
    return true;
}

Action* ActionManager::getActionById(const QString& id)
{
    for (auto& a : actions_)
    {
        if (a->id == id)
        {
            return a.get();
        }
    }
    return nullptr;
}

ActionManager ActionManager::instance_;