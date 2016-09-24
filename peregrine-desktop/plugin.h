#pragma once

#include <QDir>
#include <QString>
#include <QLibrary>
#include <list>
#include <vector>
#include <memory>
#include <functional>

class PluginModule
{
public:
    PluginModule(QDir dir, const QString& name);

    int runAction(const QString& actionId, const QString& input);
    struct SuggestionItem
    {
        QString text;
        QString imagePath;
        size_t token;
    };
    std::vector<SuggestionItem> getSuggestionItems(const QString& actionId, const QString& input);
    int runSuggestion(size_t token);

    const QString& name() const { return name_; };

private:
    QString name_;
    std::unique_ptr<QLibrary> lib_;

    // resolved functions
    typedef int(*fpRunAction)(const char* actionId, const char* data);
    typedef int(*fpGetSuggestionItems)(const char* currentActionId, const char* input, int* n, struct PG_SUGGESTION_ITEM** items);
    typedef int(*fpRunSuggestion)(size_t token);
    
    fpRunAction runActionFunc_;
    fpGetSuggestionItems getSuggestionItemsFunc_;
    fpRunSuggestion runSuggestionFunc_;
};

class PluginManager
{
public:
    bool loadPlugin(const QString& dir);
    PluginModule* loadPluginModule(const QDir& dir, const QString& name);
    void setCallbacks(std::function<int(const QString&)> setHeaderTextFunc, 
        std::function<int(const char*, const char*)> invokeQmlFuncFunc);
    static PluginManager& getInstance() { return instance_; }
    static QString getPluginNameFromDir(const QDir& dir);

private:
    PluginModule* tryGetModule(const QString& actionId);

    static PluginManager instance_;
    std::list<PluginModule> moduleList_;
};
