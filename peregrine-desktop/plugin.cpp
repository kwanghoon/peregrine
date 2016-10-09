// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#include "plugin.h"
#include "action.h"
#include <QDomDocument>
#include <QLibrary>
#include <QDebug>
#include <cassert>

using namespace std;

namespace
{
    struct PluginContext
    {
        PluginModule* currPlugin = nullptr;
    };
    PluginContext g_pluginContext;

    void loadActionList(QString path)
    {
        qDebug() << "new action list file: " << path;

        QFile actionListFile(path);
        QDir dir(path);
        dir.cdUp();
        QDomDocument doc;
        doc.setContent(&actionListFile);

        auto root = doc.documentElement();
        auto child = root.firstChildElement();
        while (!child.isNull())
        {
            LoadAction(child, dir);
            child = child.nextSiblingElement();
        }
    }

    PG_FUNC_TABLE funcTable;
    PG_PLUGIN_CONFIGS pluginConfigs;
}

PluginModule::PluginModule(QDir dir, const QString& name)
{
    // load dynamic library
    unique_ptr<QLibrary> lib(new QLibrary);
    lib->setFileName(dir.absoluteFilePath(name));
    if (!lib->load() || !lib->isLoaded())
    {        
        QString message = QString("plugin library load failed. (%1)").arg(lib->errorString());
        throw std::runtime_error(message.toStdString());
    }

    typedef int(*fpInitializePlugin)(const PG_FUNC_TABLE*, const PG_PLUGIN_CONFIGS*);
    auto initPluginFunc = (fpInitializePlugin)lib->resolve("InitializePlugin");
    g_pluginContext.currPlugin = this;
    assert(!!funcTable.fpSetHeaderText);
    int ret = initPluginFunc(&funcTable, &pluginConfigs);
    g_pluginContext.currPlugin = nullptr;
    if (ret < 0)
    {
        QString messae;
        messae.sprintf("plugin initialization failed. (%d)", ret);
        throw std::runtime_error(messae.toStdString());
    }

    // resolve library functions
    runActionFunc_ = (fpRunAction)lib->resolve("RunAction");
    getSuggestionItemsFunc_ = (fpGetSuggestionItems)lib->resolve("GetSuggestionItems");
    runSuggestionFunc_ = (fpRunSuggestion)lib->resolve("RunSuggestion");

    name_ = name;
    lib_ = std::move(lib);
}

int PluginModule::runAction(const QString& actionId, const QMap<QString, QString>& arguments)
{
    PG_ACTION_ARGUMENT_SET argsSet;
    vector<PG_ARGUMENT> args;
    vector<pair<QByteArray, QByteArray>> baArray;
    args.reserve(arguments.size());
    baArray.reserve(arguments.size());
    for (auto argIt = arguments.cbegin(); argIt != arguments.cend(); argIt++)
    {
        baArray.emplace_back(argIt.key().toUtf8(), argIt.value().toUtf8());
        args.emplace_back(PG_ARGUMENT{baArray.back().first.constData(),
            baArray.back().second.constData()});
    }
    argsSet.n = args.size();
    argsSet.arguments = argsSet.n != 0 ? &args.at(0) : nullptr;
    return runActionFunc_(actionId.toStdString().c_str(), &argsSet);
}

std::vector<PluginModule::SuggestionItem> PluginModule::getSuggestionItems(const QString& actionId, const QString& input)
{
    if (!getSuggestionItemsFunc_)
    {
        return{};
    }

    int n = 0;
    PG_SUGGESTION_ITEM* items = nullptr;
    getSuggestionItemsFunc_(actionId.toStdString().c_str(), input.toStdString().c_str(), &n, &items);

    std::vector<SuggestionItem> ret;
    for (int i = 0; i < n; i++)
    {
        ret.push_back({ QString::fromUtf8(items[i].displayText), items[i].imagePath, 
            items[i].token, items[i].completeText });
        ::free(items[i].displayText);
        ::free(items[i].imagePath);
        ::free(items[i].completeText);
    }
    ::free(items);

    return ret;
}

int PluginModule::runSuggestion(size_t token)
{
    return runSuggestionFunc_(token);
}

// PluginManager
bool PluginManager::loadPlugin(const QString& dir)
{
    qDebug() << "load plugin at " << dir;

    // parse xml
    QDir pluginDir(dir);
    QFile pluginSettingFile(pluginDir.absoluteFilePath("plugin.xml"));

    QDomDocument doc;
    doc.setContent(&pluginSettingFile);

    auto docElem = doc.documentElement();
    QString pluginName = docElem.attribute("name");

    for (auto child = docElem.firstChildElement("actions").firstChildElement();
        !child.isNull(); child = child.nextSiblingElement())
    {
        if (child.tagName() == "action")
        {
            LoadAction(child, pluginDir);
        }
        else
        {
            assert(false);
        }
    }

    for (auto child = docElem.firstChildElement("actionfiles").firstChildElement();
        !child.isNull(); child = child.nextSiblingElement())
    {
        assert(child.tagName() == "actionfile");
        loadActionList(pluginDir.filePath(child.attribute("path")));
    }

    return true;
}

PluginModule* PluginManager::loadPluginModule(const QDir& dir, const QString& name)
{
    auto* module = tryGetModule(name);
    if (module)
    {
        return module;
    }

    try
    {
        moduleList_.emplace_back(dir, name);
    }
    catch (std::exception ex)
    {
        return nullptr;
    }
    return &moduleList_.back();
}

void PluginManager::setCallbacks(std::function<int(const QString&)> setHeaderTextFunc, 
    std::function<int(const char*, const char*)> invokeQmlFuncFunc)
{
    static decltype(setHeaderTextFunc) s_setHeaderTextFunc;
    static decltype(invokeQmlFuncFunc) s_invokeQmlFuncFunc;
    s_setHeaderTextFunc = setHeaderTextFunc;
    s_invokeQmlFuncFunc = invokeQmlFuncFunc;
    funcTable.fpSetHeaderText = [](const char* text) {
        s_setHeaderTextFunc(text);
        return 0;
    };
    funcTable.fpInvokeQmlFunc = [](const char* funcName, const char* jsonArg) {
        s_invokeQmlFuncFunc(funcName, jsonArg);
        return 0;
    };
}

void PluginManager::setConfigs(int maxSuggestions)
{
    pluginConfigs.maxSuggestions = maxSuggestions;
}

QString PluginManager::getPluginNameFromDir(const QDir& dir)
{
    QFile pluginSettingFile(dir.absoluteFilePath("plugin.xml"));

    QDomDocument doc;
    doc.setContent(&pluginSettingFile);

    auto docElem = doc.documentElement();
    return docElem.attribute("name");    
}

PluginModule* PluginManager::tryGetModule(const QString& actionId)
{
    for (auto& p : moduleList_)
    {
        if (p.name() == actionId)
        {
            return &p;
        }
    }
    return nullptr;
}

PluginManager PluginManager::instance_;
