#include "action.h"
#include "plugin.h"
#include <stringhelper.h>
#include <Jinja2CppLight.h>
#include <exprtk/exprtk.hpp>

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
        return true;
    }

    for (auto& todo : doList)
    {
        auto action = ActionManager::getInstance().getActionById(todo.actionId);

        if (!todo.condition.isEmpty())
        {
            exprtk::symbol_table<double> st;
            {
                std::string input_s = input.toStdString();
                st.add_stringvar("input_text", input_s, true);
            }
            exprtk::expression<double> expr;
            expr.register_symbol_table(st);
            exprtk::parser<double> p;
            p.compile(todo.condition.toStdString(), expr);
            bool result = expr.value();
            if (!result)
            {
                continue;
            }
        }

        Jinja2CppLight::Template inputTemplate(todo.inputTemplate.toStdString());
        inputTemplate.setValue("input_text", input.toStdString());
        action->run(QString::fromStdString(inputTemplate.render()));
    }
    return true;
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