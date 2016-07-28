#pragma once

#include <QString>
#include <memory>
#include <list>

class Plugin;

class Action
{
public:
    bool run(const QString& input);

    QString id;
    QString imagePath;
    Plugin* plugin = nullptr;
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