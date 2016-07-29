#include "inputhandlerdelegate.h"
#include "launcherwindow.h"
#include "ui_launcherwindow.h"
#include "action.h"
#include "plugin.h"
#include "inputhandlerdelegate.h"
#include <peregrine-plugin-sdk.h>
#include <QXmlSimpleReader>
#include <QQmlContext>
#include <QQuickItem>
#include <QKeyEvent>
#include <QLibrary>
#include <QDebug>
#include <QDir>
#include <memory>

using namespace std;

namespace
{
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
    ui->inputContainer->setSource(QUrl::fromLocalFile("inputcontainer.qml"));

    auto* context = ui->inputContainer->rootContext();
    inputHandlerDelegate_ = new InputHandlerDelegate(this);
    context->setContextProperty("inputHandlerDelegate", inputHandlerDelegate_);

    loadSetting();
    loadPlugins();
    
    vector<ActionSelectDialog::ActionAssignInfo> v;
    for (auto& a : settings.actionSlotAssignData)
    {
        v.emplace_back();
        v.back().id = a.actionId;
        v.back().imagePath = ActionManager::getInstance().getActionById(a.actionId)->imagePath;
        v.back().pos = a.pos;
    }
    actionSelectDlg_.setActionAssignInfo(v);
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
        actionSelectDlg_.exec();

        QString actionId = actionSelectDlg_.getSelectedAction();
        inputHandlerDelegate_->currentAction = actionId;

        // focus on 'inputText' element.
        ui->inputContainer->setFocus();

        auto* item = ui->inputContainer->rootObject();
        auto* textInput = dynamic_cast<QQuickItem*>(item->children()[0]);
        textInput->forceActiveFocus();
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
        PluginManager::getInstance().loadPlugin(pluginDir.absoluteFilePath(subdir.path()));
    }
}

void LauncherWindow::tryLoadPlugin(QString path)
{
    qDebug() << "load plugin at " << path;
    PluginManager::getInstance().loadPlugin(path);
}
