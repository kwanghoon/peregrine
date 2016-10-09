// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#pragma once

#include <peregrine-plugin-sdk.h>
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

    int runAction(const QString& actionId, const QMap<QString, QString>& arguments);
    struct SuggestionItem
    {
        QString text;
        QString imagePath;
        size_t token;
        QString completeText;
    };
    std::vector<SuggestionItem> getSuggestionItems(const QString& actionId, const QString& input);
    int runSuggestion(size_t token);

    const QString& name() const { return name_; };

private:
    QString name_;
    std::unique_ptr<QLibrary> lib_;

    // resolved functions
    typedef int(*fpRunAction)(const char* actionId, const PG_ACTION_ARGUMENT_SET* argumentSet);
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
    void setConfigs(int maxSuggestions);
    static PluginManager& getInstance() { return instance_; }
    static QString getPluginNameFromDir(const QDir& dir);

private:
    PluginModule* tryGetModule(const QString& actionId);

    static PluginManager instance_;
    std::list<PluginModule> moduleList_;
};
