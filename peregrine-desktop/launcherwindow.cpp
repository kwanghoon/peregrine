#include "inputhandlerdelegate.h"
#include "launcherwindow.h"
#include "ui_launcherwindow.h"
#include "inputhandlerdelegate.h"
#include "suggestionlistcontroller.h"
#include "actionuihelper.h"
#include "action.h"
#include "plugin.h"
#include "global.h"
#include <peregrine-plugin-sdk.h>
#include <QXmlSimpleReader>
#include <QQmlContext>
#include <QQuickItem>
#include <QKeyEvent>
#include <QLibrary>
#include <QDebug>
#include <QDir>
#include <memory>
#include <cassert>

using namespace std;

LauncherWindow::LauncherWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LauncherWindow)
{
    ui->setupUi(this);
    ui->inputContainer->setSource(QUrl::fromLocalFile("inputcontainer.qml"));

    auto* context = ui->inputContainer->rootContext();
    inputHandlerDelegate_ = new InputHandlerDelegate(this);
    {
        connect(inputHandlerDelegate_, &InputHandlerDelegate::onInputTextAccepted, this, &LauncherWindow::onInputTextAccepted);
        connect(inputHandlerDelegate_, &InputHandlerDelegate::onInputTextChanged, this, &LauncherWindow::onInputTextChanged);
        connect(inputHandlerDelegate_, &InputHandlerDelegate::onSuggestionItemClicked, this, &LauncherWindow::onSuggestionItemClicked);
    }
    context->setContextProperty("inputHandlerDelegate", inputHandlerDelegate_);

    initSuggestionListController();
    loadSetting();
    loadPlugins();
    
    vector<ActionSelectDialog::ActionAssignInfo> v;
    for (auto& a : global::userConfig.actionSlotAssignData)
    {
        auto* action = ActionManager::getInstance().getActionById(a.actionId);
        assert(action);
        v.emplace_back();
        v.back().id = a.actionId;
        v.back().imagePath = action->imagePath;
        v.back().pos = a.pos;
    }
    actionSelectDlg_.setActionAssignInfo(v);

    setFocus();
}

LauncherWindow::~LauncherWindow()
{
    delete ui;
}

void LauncherWindow::initSuggestionListController()
{
    QQuickItem* suggestionListView = ui->inputContainer->rootObject()->findChild<QQuickItem*>("suggestionListView");
    assert(!!suggestionListView);

    QObject* suggestionModel = ui->inputContainer->rootObject()->findChild<QObject*>("suggestionModel");
    assert(!!suggestionModel);

    global::suggestionListController = new SuggestionListController(suggestionListView, suggestionModel, inputHandlerDelegate_);
}

void LauncherWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Shift)
    {
        QWidget* actionDisplay = ui->centralWidget->findChild<QWidget*>("actionDisplay");
        
        actionSelectDlg_.moveForSelectionDisplay(this->mapToGlobal(actionDisplay->pos()));
        actionSelectDlg_.exec();

        QString actionId = actionSelectDlg_.getSelectedActionId();
        changeAction(actionId);

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
                global::userConfig.pluginDir = atts.value("plugindir");
                qDebug() << global::userConfig.pluginDir;
            }
            else if (localName == "actionslot")
            {
                UserConfig::ActionSlotAssignInfo slot;
                {
                    slot.actionId = atts.value("actionid");
                    slot.pos = QPoint(atts.value("x").toInt(),
                        atts.value("y").toInt());
                }
                global::userConfig.actionSlotAssignData.push_back(slot);

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
    QDir pluginDir(global::userConfig.pluginDir);
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

void LauncherWindow::changeAction(QString actionId)
{
    currentAction_ = actionId;

    // #TODO: replace the selected action image
    QLabel* actionDisplay = ui->centralWidget->findChild<QLabel*>("actionDisplay");
    auto action = ActionManager::getInstance().getActionById(actionId);
    ActionUIHelper::loadActionImage(actionDisplay, action->imagePath, action->name);
}

void LauncherWindow::onInputTextAccepted(const QString& inputText)
{
    qDebug() << inputText;
    auto action = ActionManager::getInstance().getActionById(currentAction_);
    action->run(inputText);
}

void LauncherWindow::onInputTextChanged(const QString& inputText)
{
    Action* currAction = ActionManager::getInstance().getActionById(currentAction_);
    global::suggestionListController->clearList();
    if (inputText.isEmpty())
    {
        return;
    }
    for (auto& l : currAction->links)
    {
        if (l.keyword.startsWith(inputText))
        {
            Action* linkedAction = ActionManager::getInstance().getActionById(l.linkedActionId);
            if (!linkedAction)
            {
                continue;
            }

            QString s = "Move to '" + linkedAction->name + "' Action";
            auto handler = [this](boost::any data) {
                QString linkedActionId = boost::any_cast<QString>(data);
                changeAction(linkedActionId);
            };
            global::suggestionListController->addItem(s, handler, linkedAction->id);
        }
    }

    // plugin provided suggestions
    if (currAction->controller)
    {
        auto suggestions = currAction->controller->getSuggestionItems(currentAction_, inputText);
        for (auto& sugg : suggestions)
        {
            global::suggestionListController->addItem(sugg.first, [](boost::any) {},  sugg.second);
        }
    }
}

void LauncherWindow::onSuggestionItemClicked(int index)
{
    qDebug() << "suggestion item (" << index << ") clicked";
}