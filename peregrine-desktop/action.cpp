#include "action.h"
#include "plugin.h"
#include <stringhelper.h>
#include <Jinja2CppLight.h>
#include <exprtk/exprtk.hpp>
#include <cassert>

using namespace std;

bool Action::run(const QString& input)
{
    if (controller)
    {
        int ret = controller->runAction(id, input);
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

const std::list<std::unique_ptr<Action>>& ActionManager::getActionList() const
{
    return actions_;
}

ActionManager ActionManager::instance_;

void LoadAction(QDomElement actionElem, QDir dir)
{
    actionElem.attribute("name");
    std::unique_ptr<Action> currentAction(new Action);
    currentAction->id = actionElem.attribute("id");
    currentAction->name = actionElem.attribute("name");
    QString v = actionElem.attribute("image");
    if (!v.isEmpty())
    {
        currentAction->imagePath = dir.absoluteFilePath(v);
    }

    if (actionElem.hasAttribute("controller"))
    {
        currentAction->controller = PluginManager::getInstance()
            .loadPluginModule(dir, actionElem.attribute("controller"));
    }

    if (actionElem.hasAttribute("ui"))
    {
        currentAction->customUiPath = dir.filePath(actionElem.attribute("ui"));
    }

    // adopt
    auto adopt = actionElem.firstChildElement("adopt");
    if (!adopt.isNull())
    {
        currentAction->adopt = adopt.attribute("id");
        
        for (auto arg = adopt.firstChildElement("arguments").firstChildElement("arg");
            !arg.isNull(); arg = arg.nextSiblingElement("arg"))
        {
            arg.attribute("name");
            currentAction->args = arg.text();
        }
    }

    // do
    for (auto child = actionElem.firstChildElement("do").firstChildElement();
        !child.isNull(); child = child.nextSiblingElement())
    {
        assert(child.tagName() == "runaction");
        Action::DoEntry e;
        {
            e.actionId = child.attribute("id");
            assert(!e.actionId.isEmpty());
            e.inputTemplate = child.attribute("input");
            if (child.hasAttribute("condition"))
            {
                e.condition = child.attribute("condition");
            }
        }
        currentAction->doList.push_back(e);
    }

    // links
    for (auto child = actionElem.firstChildElement("links").firstChildElement();
        !child.isNull(); child = child.nextSiblingElement())
    {
        assert(child.tagName() == "link");
        Action::ActionLinkEntry e;
        {
            e.linkedActionId = child.attribute("actionid");
            assert(!e.linkedActionId.isEmpty());
            e.keyword = child.attribute("keyword");
            e.inputText = child.attribute("input_text");
        }
        currentAction->links.push_back(e);
    }

    ActionManager::getInstance().addAction(move(currentAction));
}
