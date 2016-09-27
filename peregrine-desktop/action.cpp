#include "action.h"
#include "plugin.h"
#include <stringhelper.h>
#include <Jinja2CppLight.h>
#include <exprtk/exprtk.hpp>
#include <cassert>

using namespace std;

int Action::run(const QString& input)
{
    int actionReturnValue = 0;
    if (controller)
    {
        actionReturnValue = controller->runAction(id, input);
        if (actionReturnValue < 0)
        {
            return actionReturnValue;
        }
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
        actionReturnValue = action->run(QString::fromStdString(inputTemplate.render()));
        if (actionReturnValue < 0)
        {
            break;
        }
    }
    return actionReturnValue;
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
    std::unique_ptr<Action> action(new Action);
    action->id = actionElem.attribute("id");
    action->name = actionElem.attribute("name");
    action->headerText = actionElem.attribute("header");
    QString v = actionElem.attribute("image");
    if (!v.isEmpty())
    {
        action->imagePath = dir.absoluteFilePath(v);
    }

    if (actionElem.hasAttribute("controller"))
    {
        action->controller = PluginManager::getInstance()
            .loadPluginModule(dir, actionElem.attribute("controller"));
    }

    if (actionElem.hasAttribute("ui"))
    {
        action->customUiPath = dir.filePath(actionElem.attribute("ui"));
    }

    // ui size
    bool ok = false;
    action->uiWidth = actionElem.attribute("ui_width", "800").toInt(&ok);
    assert(ok);
    action->uiHeight = actionElem.attribute("ui_height", "600").toInt(&ok);
    assert(ok);

    // adopt
    auto adopt = actionElem.firstChildElement("adopt");
    if (!adopt.isNull())
    {
        action->adopt = adopt.attribute("id");
        
        for (auto arg = adopt.firstChildElement("arguments").firstChildElement("arg");
            !arg.isNull(); arg = arg.nextSiblingElement("arg"))
        {
            arg.attribute("name");
            action->args = arg.text();
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
        action->doList.push_back(e);
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
        action->links.push_back(e);
    }

    ActionManager::getInstance().addAction(move(action));
}
