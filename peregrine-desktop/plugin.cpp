#include "plugin.h"
#include "action.h"
#include <peregrine-plugin-sdk.h>
#include <QXmlSimpleReader>
#include <QLibrary>
#include <QDebug>
#include <cassert>

using namespace std;

namespace
{
    struct ActionListContentHandler : QXmlDefaultHandler
    {
        ActionListContentHandler(QDir dir)
            : QXmlDefaultHandler()
            , dir_(dir)
        {}

        bool startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts) override
        {
            if (localName == "action")
            {
                currentAction_.reset(new Action);
                currentAction_->id = atts.value("id");
                currentAction_->name = atts.value("name");
                currentAction_->imagePath = dir_.absoluteFilePath(atts.value("image"));
            }
            else if (localName == "link")
            {
                assert(!!currentAction_);
                Action::ActionLinkEntry e;
                {
                    e.linkedActionId = atts.value("actionid");
                    e.keyword = atts.value("keyword");
                }
                currentAction_->links.push_back(e);
            }
            else if (localName == "runaction")
            {
                Action::DoEntry e;
                {
                    e.actionId = atts.value("id");
                    e.inputTemplate = atts.value("input");
                    if (atts.index("condition") != -1)
                    {
                        e.condition = atts.value("condition");
                    }
                }
                currentAction_->doList.push_back(e);
            }
            return true;
        }

        virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName) override
        {
            if (localName == "action")
            {
                assert(!!currentAction_);
                ActionManager::getInstance().addAction(move(currentAction_));
            }
            return true;
        }

        std::unique_ptr<Action> currentAction_;
        QDir dir_;
    };
    struct PluginSettingContentHandler : QXmlDefaultHandler
    {
        QString pluginName;

        PluginSettingContentHandler(QDir pluginDir)
            : QXmlDefaultHandler()
            , pluginDir_(pluginDir)
        {}

        bool startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts) override
        {
            if (localName == "plugin")
            {
                pluginName = atts.value("name");
            }
            else if (localName == "actionfile")
            {
                loadActionList(pluginDir_.filePath(atts.value("path")));
            }
            return true;
        }

        void loadActionList(QString path)
        {
            qDebug() << "new action list file: " << path;

            QXmlSimpleReader xmlReader;
            QFile actionListFile(path);
            unique_ptr<QXmlInputSource> source(new QXmlInputSource(&actionListFile));
            QDir dir(path);
            dir.cdUp();
            unique_ptr<ActionListContentHandler> contentHandler(new ActionListContentHandler(dir));
            xmlReader.setContentHandler(contentHandler.get());
            if (!xmlReader.parse(source.get()))
            {
                throw std::runtime_error((string)"Parsing failed. " + path.toLocal8Bit().toStdString());
            }
        }

    private:
        QDir pluginDir_;
    };

    struct PluginContext
    {
        Plugin* currPlugin = nullptr;
    };
    PluginContext g_pluginContext;
    int __stdcall Plugin_RegisterAction(const char* id)
    {
        qDebug() << "action (" << id << ") registered!";
        std::unique_ptr<Action> action(new Action);
        action->id = id;
        action->plugin = g_pluginContext.currPlugin;
        ActionManager::getInstance().addAction(move(action));
        return 0;
    }
}

Plugin::Plugin(const QString& path)
{
    qDebug() << "load plugin at " << path;

    // parse xml
    QDir pluginDir(path);
    QFile pluginSettingFile(pluginDir.absoluteFilePath("plugin.xml"));
    QXmlSimpleReader xmlReader;
    unique_ptr<QXmlInputSource> source(new QXmlInputSource(&pluginSettingFile));
    unique_ptr<PluginSettingContentHandler> contentHandler(new PluginSettingContentHandler(pluginDir));
    xmlReader.setContentHandler(contentHandler.get());
    if (!xmlReader.parse(source.get()))
    {
        throw std::runtime_error("Parsing failed. (plugin.xml)");
    }

    // load dynamic library
    QString pluginName = contentHandler->pluginName;
    unique_ptr<QLibrary> lib(new QLibrary);
    lib->setFileName(pluginDir.absoluteFilePath(pluginName));
    if (!lib->load() || !lib->isLoaded())
    {
        QString messae;
        messae.sprintf("plugin library load failed. (%s)", lib->errorString());
        throw std::runtime_error(messae.toStdString());
    }

    // call plugin's initialize function
    PG_FUNC_TABLE table;
    table.fpRegisterAction = Plugin_RegisterAction;

    typedef int(*fpInitializePlugin)(const PG_FUNC_TABLE*);
    auto initPluginFunc = (fpInitializePlugin)lib->resolve("InitializePlugin");
    g_pluginContext.currPlugin = this;
    int ret = initPluginFunc(&table);
    g_pluginContext.currPlugin = nullptr;
    if (ret < 0)
    {
        QString messae;
        messae.sprintf("plugin initialization failed. (%d)", ret);
        throw std::runtime_error(messae.toStdString());
    }

    name_ = pluginName;
    lib_ = std::move(lib);
}

bool Plugin::runAction(const QString& actionId, const QString& input)
{
    typedef int(*fpRunAction)(const char* actionId, const char* data);
    auto runActionFunc = (fpRunAction)lib_->resolve("RunAction");
    runActionFunc(actionId.toStdString().c_str(), input.toStdString().c_str());
    return true;
}

// PluginManager

bool PluginManager::loadPlugin(const QString& dir)
{
    try
    {
        pluginList_.emplace_back(dir);
    }
    catch (std::exception ex)
    {
        return false;
    }
    return true;
}

Plugin* PluginManager::getPluginForAction(const QString& actionId)
{
    for (auto& p : pluginList_)
    {
        if (p.name() == actionId)
        {
            return &p;
        }
    }
    return nullptr;
}

PluginManager PluginManager::instance_;
