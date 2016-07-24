#include "launcherwindow.h"
#include "ui_launcherwindow.h"
#include <QKeyEvent>
#include <QDebug>
#include <QPushButton>
#include <QWidget>
#include <QXmlSimpleReader>
#include <QDir>
#include <memory>

using namespace std;

namespace
{
    struct ActionInfo
    {
        QString id;
        QString imagePath;
    };
    vector<ActionInfo> actionList;

    ActionInfo* getActionById(const QString& id)
    {
        for (auto& a : actionList)
        {
            if (a.id == id)
            {
                return &a;
            }
        }
        return nullptr;
    }

    struct ActionSlotAssignInfo
    {
        QPoint pos;
        QString actionId;
    };

    struct PeregrineSettings
    {
        QString pluginDir;
        vector<ActionSlotAssignInfo> actionSlotAssignData;
    } settings;
}

LauncherWindow::LauncherWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LauncherWindow)
{
    ui->setupUi(this);
    loadSetting();
    loadPlugins();
}

LauncherWindow::~LauncherWindow()
{
    delete ui;
}

void LauncherWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Shift)
    {
        auto* btn = ui->centralWidget->findChild<QPushButton*>("pushButton");
        
        actionSelectDlg_.moveForSelectionDisplay(this->mapToGlobal(btn->pos()));
        actionSelectDlg_.show();
    }
}

void LauncherWindow::loadSetting()
{
    struct SettingXMLContentHandler : QXmlDefaultHandler
    {
        SettingXMLContentHandler() : QXmlDefaultHandler() {}

        bool startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts) override
        {
            if (localName == "plugin")
            {
                settings.pluginDir = atts.value("plugindir");
                qDebug() << settings.pluginDir;
            }
            else if (localName == "actionslot")
            {
                ActionSlotAssignInfo slot;
                {
                    slot.actionId = atts.value("actionid");
                    slot.pos = QPoint(atts.value("x").toInt(),
                        atts.value("y").toInt());
                }
                settings.actionSlotAssignData.push_back(slot);

            }
            return true;
        }
    };
    QXmlSimpleReader xmlReader;
    QFile settingFile("settings.xml");
    unique_ptr<QXmlInputSource> source(new QXmlInputSource(&settingFile));
    unique_ptr<SettingXMLContentHandler> contentHandler(new SettingXMLContentHandler);
    xmlReader.setContentHandler(contentHandler.get());
    if (!xmlReader.parse(source.get()))
    {
        throw std::runtime_error("Parsing failed. (settings.xml)");
    }
}

void LauncherWindow::loadPlugins()
{
    QDir pluginDir(settings.pluginDir);
    auto subdirs = pluginDir.entryList(QDir::Filter::Dirs | QDir::Filter::NoDotAndDotDot);
    for (QDir subdir : subdirs)
    {
        auto xmlPath = pluginDir.absoluteFilePath(subdir.filePath("plugin.xml"));
        if (!QFile::exists(xmlPath))
        {
            continue;
        }
        loadPlugin(pluginDir.absoluteFilePath(subdir.path()));
    }
}

void LauncherWindow::loadPlugin(QString path)
{
    qDebug() << "load plugin at " << path;

    QDir pluginDir(path);
    QFile pluginSettingFile(pluginDir.absoluteFilePath("plugin.xml"));
    QXmlSimpleReader xmlReader;
    unique_ptr<QXmlInputSource> source(new QXmlInputSource(&pluginSettingFile));
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
                ActionInfo info;
                {
                    info.id = atts.value("id");
                    info.imagePath = dir_.absoluteFilePath(atts.value("image"));
                }
                actionList.push_back(info);
            }
            return true;
        }

        QDir dir_;
    };
    struct PluginSettingContentHandler : QXmlDefaultHandler
    {
        PluginSettingContentHandler(QDir pluginDir) 
            : QXmlDefaultHandler()
            , pluginDir_(pluginDir)
        {}

        bool startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts) override
        {
            if (localName == "actionfile")
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

        QDir pluginDir_;
    };

    unique_ptr<PluginSettingContentHandler> contentHandler(new PluginSettingContentHandler(pluginDir));
    xmlReader.setContentHandler(contentHandler.get());
    if (!xmlReader.parse(source.get()))
    {
        throw std::runtime_error("Parsing failed. (plugin.xml)");
    }
}
