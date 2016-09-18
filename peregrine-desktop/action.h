#pragma once

#include <QDomDocument>
#include <QString>
#include <QDir>
#include <memory>
#include <list>
#include <vector>

class PluginModule;

class Action
{
public:
    bool run(const QString& input);

    QString id;
    QString name;
    QString imagePath;
    PluginModule* controller = nullptr;
    QString customUiPath;
    QString adopt;
    QString args;

    struct DoEntry
    {
        QString actionId;
        QString condition;
        QString inputTemplate;
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