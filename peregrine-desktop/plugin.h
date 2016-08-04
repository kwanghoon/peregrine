#pragma once

#include <QDir>
#include <QString>
#include <QLibrary>
#include <list>
#include <vector>
#include <memory>

class Plugin
{
public:
    Plugin(const QString& path);
    bool runAction(const QString& actionId, const QString& input);
    const QString& name() const { return name_; };

private:
    QString name_;
    std::unique_ptr<QLibrary> lib_;

    typedef int(*fpRunAction)(const char* actionId, const char* data);
    fpRunAction runActionFunc_;
};

class PluginManager
{
public:
    bool loadPlugin(const QString& dir);
    Plugin* getPluginForAction(const QString& actionId);
    static PluginManager& getInstance() { return instance_; }

private:
    static PluginManager instance_;
    std::list<Plugin> pluginList_;
};
