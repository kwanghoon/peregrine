// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#include "action.h"
#include "plugin.h"
#include <stringhelper.h>
#include <Jinja2CppLight.h>
#include <exprtk/exprtk.hpp>
#include <QMap>
#include <cassert>

using namespace std;

namespace
{
    QMap<QString, QString> evaluate(const QMap<QString, QString>& exprMap, const QMap<QString, QString>& argMap,
        QList<QPair<QString, QString>> vars)
    {
        QMap<QString, QString> result;
        for (auto exprIt = exprMap.cbegin(); exprIt != exprMap.cend(); exprIt++)
        {
            Jinja2CppLight::Template t(exprIt.value().toStdString());
            for (auto it = argMap.cbegin(); it != argMap.cend(); it++)
            { 
                t.setValue(it.key().toStdString(), it.value().toStdString());
            }
            Q_FOREACH(auto& e, vars)
            {
                t.setValue(e.first.toStdString(), e.second.toStdString());
            }
            result.insert(exprIt.key(), QString::fromStdString(t.render()));
        }

        return result;
    }
}

int Action::run(const QMap<QString, QString>& argumentMap, IActionContext* context)
{
    int actionReturnValue = 0;
    if (controller)
    {
        actionReturnValue = controller->runAction(id, argumentMap);
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
            std::list<std::string> stringStore;
            stringStore.push_back(argumentMap["input_text"].toStdString());
            st.add_stringvar("input_text", stringStore.back(), true);

            exprtk::expression<double> expr;
            expr.register_symbol_table(st);

            exprtk::parser<double> p;
            p.compile(todo.condition.toStdString(), expr);
            bool result = static_cast<bool>(expr.value());
            if (!result)
            {
                continue;
            }
        }

        QList<QPair<QString, QString>> vars;
        for (auto it = todo.varFuncMap.cbegin(); it != todo.varFuncMap.cend(); it++)
        {
            vars.push_back({ it.key(), context->invokeFunc(it.value()) });
        }
        auto argMap = evaluate(todo.actionArguments, argumentMap, vars);

        actionReturnValue = action->run(argMap, context);
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



namespace
{
    void parseDoElement(QDomElement &actionElem, Action* action)
    {
        for (auto child = actionElem.firstChildElement("do").firstChildElement();
            !child.isNull(); child = child.nextSiblingElement())
        {
            assert(child.tagName() == "runaction");
            Action::DoEntry e;
            {
                e.actionId = child.attribute("id");
                assert(!e.actionId.isEmpty());
                if (child.hasAttribute("condition"))
                {
                    e.condition = child.attribute("condition");
                }

                // <variables />
                auto variablesElem = child.firstChildElement("variables");
                if (!variablesElem.isNull()) 
                {
                    for (auto varElem = variablesElem.firstChildElement();
                        !varElem.isNull(); varElem = varElem.nextSiblingElement())
                    {
                        if (varElem.tagName() == "varbycall")
                        {
                            e.varFuncMap.insert(
                                varElem.attribute("name"), varElem.attribute("func"));
                        }
                    }
                }

                // <arguments />
                auto argsElem = child.firstChildElement("arguments");
                if (!argsElem.isNull()) 
                { 
                    for (auto argElem = argsElem.firstChildElement("arg");
                        !argElem.isNull(); argElem = argElem.nextSiblingElement("arg"))
                    {
                        e.actionArguments.insert(
                            argElem.attribute("name"), argElem.attribute("value"));
                    }
                }
            }
            action->doList.push_back(e);
        }
    }

    void parseLinksElement(QDomElement &actionElem, Action* action)
    {
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
    }
}

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

    parseDoElement(actionElem, action.get());
    parseLinksElement(actionElem, action.get());

    ActionManager::getInstance().addAction(move(action));
}
