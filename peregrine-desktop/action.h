// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#pragma once

#include <QDomDocument>
#include <QString>
#include <QDir>
#include <QMap>
#include <memory>
#include <list>
#include <vector>

class PluginModule;

class Action
{
public:
    int run(const QMap<QString, QString>& argumentMap);
    bool hasCustomUI() const { return !customUiPath.isEmpty(); }

    QString id;
    QString name;
    QString imagePath;
    PluginModule* controller = nullptr;
    QString customUiPath;
    int uiWidth, uiHeight;
    QString adopt;
    QString args;
    QString headerText;

    struct DoEntry
    {
        QString actionId;
        QString condition;
        QMap<QString, QString> actionArguments;
    };
    std::vector<DoEntry> doList;

    struct ActionLinkEntry
    {
        QString linkedActionId;
        QString keyword;
        QString inputText;
    };
    std::vector<ActionLinkEntry> links;
};

class ActionManager
{
public:
    bool addAction(std::unique_ptr<Action> action);
    Action* getActionById(const QString& id);

    // #TODO: thread-safety
    const std::list<std::unique_ptr<Action>>& getActionList() const;
    static ActionManager& getInstance() { return instance_; }
    
private:
    std::list<std::unique_ptr<Action>> actions_;
    static ActionManager instance_;
};

void LoadAction(QDomElement actionElem, QDir dir);