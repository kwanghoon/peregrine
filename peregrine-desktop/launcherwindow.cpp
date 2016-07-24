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
    struct PeregrineSettings
    {
        QString pluginDir;
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

        bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts) override
        {
            if (localName == "plugin")
            {
                settings.pluginDir = atts.value("plugindir");
                qDebug() << settings.pluginDir;
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
    struct PluginSettingContentHandler : QXmlDefaultHandler
    {
        PluginSettingContentHandler() : QXmlDefaultHandler() {}

        bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts) override
        {
            if (localName == "actionfile")
            {
                qDebug() << "new action file: " << atts.value("path");
            }
            return true;
        }
    };

    unique_ptr<PluginSettingContentHandler> contentHandler(new PluginSettingContentHandler);
    xmlReader.setContentHandler(contentHandler.get());
    if (!xmlReader.parse(source.get()))
    {
        throw std::runtime_error("Parsing failed. (plugin.xml)");
    }
}
