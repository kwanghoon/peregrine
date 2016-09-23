#include "inputhandlerdelegate.h"
#include "launcherwindow.h"
#include "ui_launcherwindow.h"
#include "configurationwindow.h"
#include "inputhandlerdelegate.h"
#include "suggestionlistcontroller.h"
#include "syncmanager.h"
#include "actionuihelper.h"
#include "action.h"
#include "plugin.h"
#include "global.h"
#include "configmanager.h"
#include <peregrine-plugin-sdk.h>
#include <QQmlContext>
#include <QQuickItem>
#include <QKeyEvent>
#include <QLibrary>
#include <QDebug>
#include <QDir>
#include <QQmlEngine>
#include <QQmlProperty>
#include <QSystemTrayIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QTimer>
#include <QClipboard>
#include <boost/range.hpp>
#include <boost/range/join.hpp>
#include <memory>
#include <cassert>

#ifdef Q_OS_WIN
#   include <windows.h>
#endif // Q_OS_WIN

using namespace std;

LauncherWindow::LauncherWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LauncherWindow)
{
    //setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);
    ui->inputContainer->setSource(QUrl::fromLocalFile("inputcontainer.qml"));
    ui->suggestionList->setSource(QUrl::fromLocalFile("SuggestionListView.qml"));
    ui->customUi->setSource(QUrl::fromLocalFile("CustomUi.qml"));

    QIcon appIcon("heart.png");
    setWindowIcon(appIcon);

    setupTrayIcon();

    inputHandlerDelegate_ = new InputHandlerDelegate(this);
    {
        connect(inputHandlerDelegate_, &InputHandlerDelegate::onInputTextChanged, this, &LauncherWindow::onInputTextChanged);
        connect(inputHandlerDelegate_, &InputHandlerDelegate::onKeyPressed, this, &LauncherWindow::onKeyPressed);
    }
    auto* context = ui->inputContainer->rootContext();
    context->setContextProperty("inputHandlerDelegate", inputHandlerDelegate_);
    context = ui->suggestionList->rootContext();
    context->setContextProperty("inputHandlerDelegate", inputHandlerDelegate_);

    initSuggestionListController();
    global::GetConfigManager().loadConfig();
    loadPlugins();
    onConfigUpdated();
    connect(&global::GetConfigManager(), &ConfigManager::onConfigUpdated, [this]() {
        onConfigUpdated();
    });

    auto thenFunc = [](const QJsonObject& configs) {
        global::GetConfigManager().updateConfig(configs.toVariantMap(), "sync");
    };
    auto catchFunc = []() {};
    auto& accountInfo = global::GetConfigManager().getAccountInfo();
    if (accountInfo.filled)
    {
        global::GetSyncManager().login(accountInfo.email, accountInfo.passwordHash,
            thenFunc, catchFunc);
    }

    setFocus();

    initHistory();

#   ifdef Q_OS_WIN
    if (::RegisterHotKey((HWND)winId(), kHotKey_PopUpId_, MOD_ALT | MOD_CONTROL | MOD_NOREPEAT,
        VK_OEM_2)) // VK_OEM_2 is '/'
    {}
#   ifndef NDEBUG
    // Ctrl + `: 
    if (::RegisterHotKey((HWND)winId(), kHotKey_ExitId_, MOD_CONTROL | MOD_NOREPEAT,
        VK_OEM_3)) // VK_OEM_3 is '`'
    {}
    // Ctrl + 1 to open Configuration window. debug only.
    if (::RegisterHotKey((HWND)winId(), kHotKey_ConfigId_, MOD_CONTROL | MOD_NOREPEAT,
        '1'))
    {}
#   endif
#   endif
}

void LauncherWindow::onConfigUpdated()
{
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
}

LauncherWindow::~LauncherWindow()
{
    delete ui;

    ::UnregisterHotKey((HWND)winId(), kHotKey_PopUpId_);
#   ifndef NDEBUG
    ::UnregisterHotKey((HWND)winId(), kHotKey_ExitId_);
    ::UnregisterHotKey((HWND)winId(), kHotKey_ConfigId_);
#   endif
}

void LauncherWindow::initSuggestionListController()
{
    ui->suggestionList->setParent(this);

    QTimer::singleShot(100, this, &LauncherWindow::updateSuggestionListPosition);

    QQuickItem* suggestionListView = ui->suggestionList->rootObject();
    QString s = suggestionListView->objectName();
    assert(!!suggestionListView);
    assert(suggestionListView->objectName() == "suggestionListView");
    
    QObject* suggestionModel = suggestionListView->findChild<QObject*>("suggestionModel");
    assert(!!suggestionModel);

    global::suggestionListController = new SuggestionListController(
        ui->suggestionList, suggestionListView, suggestionModel, inputHandlerDelegate_);
}

void LauncherWindow::updateSuggestionListPosition()
{
    QPoint pos = ui->inputContainer->mapTo(this, QPoint(0, 0));
    pos.ry() += ui->inputContainer->geometry().height();
    ui->suggestionList->move(pos);
    ui->suggestionList->setFixedWidth(ui->inputContainer->width());
}

void LauncherWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key::Key_Control || event->key() == Qt::Key::Key_Alt)
    {
        int keyCombinationFlags = (Qt::Modifier::CTRL | Qt::Modifier::ALT);
        if ((event->modifiers() & keyCombinationFlags) == keyCombinationFlags)
        {
            showActionSelectDialog();
            return;
        }
    }
    
    if (event->key() == Qt::Key::Key_Escape)
    {
        pushDown();
    }
    else if (event->key() == Qt::Key::Key_Z && (event->modifiers() & Qt::Modifier::CTRL) != 0)
    {
        if ((event->modifiers() & Qt::Modifier::SHIFT) != 0)
        {
            qDebug() << "redo";
        }
        else
        {
            if (!actionHistory_.empty())
            {
                if (inputHistoryIterator_ != actionHistory_.begin())
                {
                    inputHistoryIterator_--;
                }
                auto* textInput = ui->inputContainer->rootObject();
                textInput->setProperty("text", *inputHistoryIterator_);
            }
        }
    }
    else if (event->key() == Qt::Key::Key_F12 && (event->modifiers() & Qt::Modifier::ALT) != 0)
    {
        appExit_ = true;
        close();
    }
}

void LauncherWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        holdingWindow_ = true;
        mousePressPos_.rx() = event->x();
        mousePressPos_.ry() = event->y();
    }
}

void LauncherWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        holdingWindow_ = false;
    }
}

void LauncherWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (holdingWindow_)
    {
        move(event->globalX() - mousePressPos_.x(), event->globalY() - mousePressPos_.y());
    }
}

void LauncherWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateSuggestionListPosition();
}

void LauncherWindow::showActionSelectDialog()
{
    QWidget* actionDisplay = ui->centralWidget->findChild<QWidget*>("actionDisplay");

    actionSelectDlg_.moveForSelectionDisplay(this->mapToGlobal(actionDisplay->pos()));
    actionSelectDlg_.exec();

    QString actionId = actionSelectDlg_.getSelectedActionId();
    if (currentAction_ != actionId)
    {
        changeAction(actionId);
        saveHistory();
    }

    // focus on 'inputText' element.
    ui->inputContainer->setFocus();

    auto* textInput = ui->inputContainer->rootObject();
    textInput->forceActiveFocus();
}

void LauncherWindow::loadPlugins()
{
    //
    PluginManager::getInstance().setCallbacks([this](const QString& text) {
        this->setHeaderText(text);
        return 0;
    }, [this](const char* functionName, const char* jsonArgument) {
        auto* customUiItem = this->ui->customUi->rootObject();
        auto* userUi = dynamic_cast<QQuickItem*>(customUiItem->children()[0]);
        QMetaObject::invokeMethod(userUi, functionName, Q_ARG(QVariant, jsonArgument));
        return 0;
    });

    // 
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

void LauncherWindow::changeAction(QString actionId, QString inputText)
{
    currentAction_ = actionId;

    QQuickItem* customUiItem = ui->customUi->rootObject();
    QLabel* actionDisplay = ui->centralWidget->findChild<QLabel*>("actionDisplay");

    if (currentAction_.isEmpty())
    {
        QMetaObject::invokeMethod(customUiItem, "clearChildren");
        actionDisplay->clear();
        return;
    }

    auto action = ActionManager::getInstance().getActionById(actionId);
    ActionUIHelper::loadActionImage(actionDisplay, action->imagePath, action->name);

    QString argsForActivatedEvent;
    if (!action->adopt.isEmpty())
    {
        argsForActivatedEvent = action->args;
        action = ActionManager::getInstance().getActionById(action->adopt);
    }
    if (!action->customUiPath.isEmpty())
    {
        QString qmlString;
        {
            QFile f(action->customUiPath);
            if (!f.open(QFile::ReadOnly | QFile::Text))
            {
                assert(false);
            }
            qmlString = QTextStream(&f).readAll();
        }
        QMetaObject::invokeMethod(customUiItem, "loadCustomUi", Q_ARG(QVariant, qmlString), Q_ARG(QVariant, argsForActivatedEvent));
    }

    if (!inputText.isEmpty())
    {
        auto* textInput = ui->inputContainer->rootObject();
        textInput->setProperty("text", inputText);
    }
}

void LauncherWindow::setupTrayIcon()
{
    QIcon trayIcon("heart.png");

    tray_ = new QSystemTrayIcon(this);
    tray_->setIcon(trayIcon);
    tray_->show();

    auto* menu = new QMenu(this);
    {
        showAction_ = menu->addAction("Show", [this] { popUp(); });
        hideAction_ = menu->addAction("Hide", [this] { pushDown(); });
        menu->addMenu("Run Action..");

        // shortcut toggle
        toggleShortcutAction_ = menu->addAction("Shortcut Enabled");
        toggleShortcutAction_->setCheckable(true);
        toggleShortcutAction_->setChecked(true);
        connect(toggleShortcutAction_, &QAction::triggered, [this]() {
            toggleShortcutAction_->setChecked(toggleShortcutAction_->isChecked());
        });

        menu->addSeparator();

        // SyncSetting Action
        auto syncSettingAction = [this]() {
            ConfigurationWindow config(nullptr, ConfigurationWindow::TabKind::Account);
            config.exec();
        };
        QAction* syncSettingActionMenu = menu->addAction("Not Synced", syncSettingAction);
        connect(&global::GetConfigManager(), &ConfigManager::onAccountInfoUpdated, [syncSettingActionMenu]() {
            if (global::GetConfigManager().getAccountInfo().filled)
            {
                QString text = "Synced with " + global::GetConfigManager().getAccountInfo().email;
                syncSettingActionMenu->setText(text);
            }
        });

        // Configuration Action
        auto configAction = [this]() {
            ConfigurationWindow config;
            config.exec();
        };
        menu->addAction("Configuration", configAction);
        menu->addSeparator();
        menu->addAction("Exit", [this] {
            appExit_ = true;
            close();
        });
    }
    tray_->setContextMenu(menu);
}

void LauncherWindow::popUp()
{
    this->setHidden(false);
    hideAction_->setEnabled(true);
    showAction_->setEnabled(false);

    // popup the window in front of the other windows.
#   ifdef Q_OS_WIN
    showMinimized();
    showNormal();
#   endif

    // On windows, some delays are required to have keyboard focus after 'popup'
    QTimer::singleShot(100, [this]() {
        // #DUPLICATE
        // focus on 'inputText' element.
        ui->inputContainer->setFocus();

        auto* textInput = ui->inputContainer->rootObject();
        textInput->forceActiveFocus();

        // load clipboard text if available
        QString s = QApplication::clipboard()->text();
        if (!s.isEmpty())
        {
            textInput->setProperty("text", s);
        }

        inputHistoryIterator_ = inputHistory_.end();

        bool pressing = false;
#       ifdef Q_OS_WIN
        if ((::GetKeyState(VK_LCONTROL) & 0x8000) != 0 && (::GetKeyState(VK_LMENU) & 0x8000) != 0)
        {
            pressing = true;
        }
#       endif
        if (pressing)
        {
            showActionSelectDialog();
        }
    });
}

void LauncherWindow::pushDown()
{
    this->setHidden(true);
    hideAction_->setEnabled(false);
    showAction_->setEnabled(true);
}

void LauncherWindow::setHeaderText(const QString& text)
{
    ui->headerTextLabel->setText(text);
}

void LauncherWindow::onInputTextChanged(const QString& inputText)
{
    if (currentAction_.isNull())
    {
        return;
    }

    Action* currAction = ActionManager::getInstance().getActionById(currentAction_);
    Action* adoptedAction = nullptr;
    if (!currAction->adopt.isEmpty())
    {
        adoptedAction = ActionManager::getInstance().getActionById(currAction->adopt);
    }
    global::suggestionListController->clearList();
    if (inputText.isEmpty())
    {
        return;
    }
    auto emptyRange = decltype(currAction->links){};
    auto joinedLinks = boost::join(currAction->links, 
        adoptedAction ? adoptedAction->links : emptyRange);
    for (auto& l : joinedLinks)
    {
        if (l.keyword.startsWith(inputText, Qt::CaseInsensitive))
        {
            Action* linkedAction = ActionManager::getInstance().getActionById(l.linkedActionId);
            if (!linkedAction)
            {
                continue;
            }

            QString s = QString("<h4>Move to <font color='chocolate'><strong>%1</strong></font> Action</h4><font color='gray'>matched by '<b>%2</b>%3'</font>")
                .arg(linkedAction->name).arg(inputText).arg(l.keyword.mid(inputText.length()));
            auto handler = [this, l](boost::any) {
                changeAction(l.linkedActionId, l.inputText);
            };
            global::suggestionListController->addItem(s, handler, nullptr);
        }
    }

    if (!currAction->customUiPath.isEmpty())
    {
        auto* customUiItem = this->ui->customUi->rootObject();
        auto* userUi = dynamic_cast<QQuickItem*>(customUiItem->children()[0]);
        QMetaObject::invokeMethod(userUi, "onInputTextChanged", Q_ARG(QVariant, inputText));
    }

    // plugin provided suggestions
    auto* controller = adoptedAction ? adoptedAction->controller : currAction->controller;
    if (controller)
    {
        auto suggestions = controller->getSuggestionItems(
            adoptedAction ? adoptedAction->id : currentAction_, inputText);
        for (auto& sugg : suggestions)
        {
            global::suggestionListController->addItem(sugg.first, [](boost::any) {}, sugg.second);
        }
    }

    bool visible = global::suggestionListController->getCount() != 0;
    global::suggestionListController->setVisible(visible);
}

void LauncherWindow::onKeyPressed(int key, int modifiers, QString inputText)
{
    // undo
    if (key == Qt::Key::Key_Z && (modifiers & Qt::Modifier::CTRL) != 0)
    {
        if (!inputHistory_.empty())
        {
            if (inputHistoryIterator_ != inputHistory_.begin())
            {
                inputHistoryIterator_--;
            }
            auto* textInput = ui->inputContainer->rootObject();
            textInput->setProperty("text", *inputHistoryIterator_);
        }
    }
    else if (key == Qt::Key::Key_Control || key == Qt::Key::Key_Alt)
    {
        // retrieve focus from the input box
        int keyCombinationFlags = (Qt::Modifier::CTRL | Qt::Modifier::ALT);
        if ((modifiers & keyCombinationFlags) == keyCombinationFlags)
        {
            setFocus();
            showActionSelectDialog();
        }
    }
    else if (key == Qt::Key::Key_Return || key == Qt::Key::Key_Enter)
    {
        int currentIndex = global::suggestionListController->getCurrentIndex();
        if (currentIndex == -1)
        {
            qDebug() << inputText;
            if (currentAction_.isEmpty())
            {
                return;
            }
            auto action = ActionManager::getInstance().getActionById(currentAction_);
            int ret = action->run(inputText);

            inputHistory_.push_back(inputText);
            if (inputHistory_.size() > 10)
            {
                inputHistory_.pop_front();
            }

            if (ret == PG_BEHAVIOR_ON_RETURN::PG_DISAPPEAR)
            {
                pushDown();
            }
        }
        else
        {
            global::suggestionListController->runSelected();
            global::suggestionListController->setVisible(false);
        }
    }
    else if (key == Qt::Key::Key_Tab)
    {
        int currentIndex = global::suggestionListController->getCurrentIndex();
        if (currentIndex != -1)
        {
            global::suggestionListController->runSelected();
            global::suggestionListController->setVisible(false);
        }
    }
    else if (key == Qt::Key::Key_Up)
    {
        global::suggestionListController->selectUp();
    }
    else if (key == Qt::Key::Key_Down)
    {
        global::suggestionListController->selectDown();
    }
    else if (key == Qt::Key::Key_Escape)
    {
        pushDown();
    }
    else if (modifiers == Qt::Modifier::ALT)
    {
        if (key == Qt::Key::Key_Left)
        {
            switchToPreviousAction();
        }
        else if (key == Qt::Key::Key_Right)
        {
            switchToNextAction();
        }
    }
}

void LauncherWindow::initHistory()
{
    actionHistoryPointer_ = actionHistory_.end();
}

void LauncherWindow::saveHistory()
{
    if (actionHistory_.empty())
    {
        assert(actionHistoryPointer_ == actionHistory_.end());
        actionHistory_.push_back(currentAction_);
        actionHistoryPointer_ = actionHistory_.begin();
    }
    else
    {
        if (next(actionHistoryPointer_) != actionHistory_.end())
        {
            actionHistory_.erase(next(actionHistoryPointer_), actionHistory_.end());
        }
        actionHistory_.push_back(currentAction_);
        actionHistoryPointer_ = prev(actionHistory_.end());
    }
}

void LauncherWindow::switchToPreviousAction()
{
    if (actionHistoryPointer_ != actionHistory_.begin())
    {
        actionHistoryPointer_--;
        changeAction(*actionHistoryPointer_);
    }
}

void LauncherWindow::switchToNextAction()
{
    if (actionHistoryPointer_ != actionHistory_.end())
    {
        actionHistoryPointer_++;
        changeAction(*actionHistoryPointer_);
    }
}

void LauncherWindow::closeEvent(QCloseEvent *event)
{
    if (appExit_)
    {
        event->accept();
    }
    else
    {
        pushDown();
        event->ignore();
    }
}

bool LauncherWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#   ifdef Q_OS_WIN
    MSG* pMsg = reinterpret_cast<MSG*>(message);
    if (pMsg->message == WM_HOTKEY)
    {
        if (pMsg->wParam == kHotKey_PopUpId_)
        {
            if (toggleShortcutAction_->isChecked())
            {
                popUp();
            }
        }
#   ifndef NDEBUG
        else if (pMsg->wParam == kHotKey_ExitId_)
        {
            appExit_ = true;
            close();
        }
        else if (pMsg->wParam == kHotKey_ConfigId_)
        {
            ConfigurationWindow config;
            config.exec();
        }
#   endif
    }
#   endif // Q_OS_WIN

    return QMainWindow::nativeEvent(eventType, message, result);
}