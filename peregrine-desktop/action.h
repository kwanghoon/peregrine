#pragma once

#include <QString>
#include <memory>
#include <list>
#include <vector>

class Plugin;

class Action
{
public:
    bool run(const QString& input);

    QString id;
    QString name;
    QString imagePath;
    Plugin* plugin = nullptr;

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
    };
    std::vector<ActionLinkEntry> links;
};

class ActionManager
{
public:
    bool addAction(std::unique_ptr<Action> action);
    Action* getActionById(const QString& id);
    static ActionManager& getInstance() { return instance_; }
    
private:
    std::list<std::unique_ptr<Action>> actions_;
    static ActionManager instance_;
};