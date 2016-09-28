#include "launcherwindow.h"
#include "inputhandlerdelegate.h"
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

// QT
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

// boost
#include <boost/range.hpp>
#include <boost/range/join.hpp>

// cpp standard libraries
#include <memory>
#include <cassert>

// platform specific
#ifdef Q_OS_WIN
#   include <windows.h>
#endif // Q_OS_WIN

using namespace std;

LauncherWindow::LauncherWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LauncherWindow)
{
    // initialize UI
    //setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);
    ui->inputContainer->setSource(QUrl::fromLocalFile("inputcontainer.qml"));
    ui->customUi->setSource(QUrl::fromLocalFile("CustomUi.qml"));
    ui->customUi->hide();
    ui->inputHistoryShowButton->setEnabled(false);
    resize(width(), ui->inputContainer->height());

    QIcon appIcon("heart.png");
    setWindowIcon(appIcon);

    setupTrayIcon();
    setFocus();

    // prepare communication with QML
    inputHandlerDelegate_ = new InputHandlerDelegate(this);
    {
        connect(inputHandlerDelegate_, &InputHandlerDelegate::onInputTextChanged, 
            this, &LauncherWindow::onInputTextChanged);
        connect(inputHandlerDelegate_, &InputHandlerDelegate::onKeyPressed, 
            this, &LauncherWindow::onKeyPressed);
    }
    auto* context = ui->inputContainer->rootContext();
    context->setContextProperty("inputHandlerDelegate", inputHandlerDelegate_);

    initSuggestionListController();

    // input history show button
    connect(ui->inputHistoryShowButton, &QPushButton::pressed, [this]() {
        global::suggestionListController->clearList();
        for (auto it = inputHistory_.crbegin(); it != inputHistory_.crend(); it++)
        {
            auto handler = [this](SuggestionListController::SuggestionRunType type, boost::any data) {
                QString pastInputText = boost::any_cast<QString>(data);

                auto* textInput = ui->inputContainer->rootObject();
                textInput->setProperty("text", pastInputText);

                return PG_BEHAVIOR_ON_RETURN::PG_REMAIN; 
            };
            global::suggestionListController->addItem(*it, QString(), handler, *it);
        }
        global::suggestionListController->setVisible(true);
    });
    connect(ui->inputHistoryShowButton, &QPushButton::released, []() {
    });

    // load configs
    global::GetConfigManager().loadConfig();
    loadPlugins();
    onConfigUpdated();
    connect(&global::GetConfigManager(), &ConfigManager::onConfigUpdated, [this]() {
        onConfigUpdated();
    });

    // prepare config synchronization
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
#   endif // NDEBUG
#   endif // Q_OS_WIN
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

#   ifdef Q_OS_WIN
    ::UnregisterHotKey((HWND)winId(), kHotKey_PopUpId_);
#   ifndef NDEBUG
    ::UnregisterHotKey((HWND)winId(), kHotKey_ExitId_);
    ::UnregisterHotKey((HWND)winId(), kHotKey_ConfigId_);
#   endif // NDEBUG
#   endif // Q_OS_WIN
}

void LauncherWindow::initSuggestionListController()
{
    suggestionBox_ = new QQuickWidget(QUrl::fromLocalFile("SuggestionListView.qml"));
    suggestionBox_->setWindowFlags(Qt::FramelessWindowHint);
    suggestionBox_->setAttribute(Qt::WA_ShowWithoutActivating);
    suggestionBox_->rootContext()->setContextProperty("inputHandlerDelegate",
        inputHandlerDelegate_);

    QTimer::singleShot(100, this, &LauncherWindow::updateSuggestionListPosition);

    QQuickItem* suggestionListView = suggestionBox_->rootObject();
    QString s = suggestionListView->objectName();
    assert(!!suggestionListView);
    assert(suggestionListView->objectName() == "suggestionListView");
    
    QObject* suggestionModel = suggestionListView->findChild<QObject*>("suggestionModel");
    assert(!!suggestionModel);

    global::suggestionListController = new SuggestionListController(
        suggestionBox_, suggestionListView, suggestionModel, inputHandlerDelegate_);
}

void LauncherWindow::updateSuggestionListPosition()
{
    QPoint pos = ui->inputContainer->mapToGlobal(QPoint(0, 0));
    pos.ry() += ui->inputContainer->geometry().height();
    suggestionBox_->move(pos);
    suggestionBox_->setFixedWidth(ui->inputContainer->width());
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
            recallInputHistory();
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
    QMetaObject::invokeMethod(customUiItem, "clearChildren");

    global::suggestionListController->setVisible(false);

    QLabel* actionDisplay = ui->centralWidget->findChild<QLabel*>("actionDisplay");

    if (currentAction_.isEmpty())
    {
        ui->headerTextLabel->setText(QString());
        actionDisplay->clear();
        return;
    }

    // set header text
    auto action = ActionManager::getInstance().getActionById(actionId);
    ActionUIHelper::loadActionImage(actionDisplay, action->imagePath, action->name);
    if (action->headerText.isEmpty())
    {
        ui->headerTextLabel->setText(QString("Type terms for '%1' action").arg(actionId));
    }
    else
    {
        ui->headerTextLabel->setText(action->headerText);
    }

    QString argsForActivatedEvent;
    int uiWidth = action->uiWidth, uiHeight = action->uiHeight;
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
        ui->customUi->resize(uiWidth, uiHeight);
        resize(uiWidth + 20, ui->inputContainer->height() + uiHeight);
        ui->customUi->show();
    }
    else
    {
        ui->customUi->hide();
        resize(width(), ui->inputContainer->height());
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

// Show the launcher window and bring it to foreground.
// As a result, users will be able to type keywords on the input box.
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

        inputHistoryIterator_ = inputHistory_.cend();

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

// Hide the launcher window. 
void LauncherWindow::pushDown()
{
    this->setHidden(true);
    hideAction_->setEnabled(false);
    showAction_->setEnabled(true);
    global::suggestionListController->setVisible(false);
}

void LauncherWindow::setHeaderText(const QString& text)
{
    ui->headerTextLabel->setText(text);
}

// It will be called when the input text is changed
void LauncherWindow::onInputTextChanged(const QString& inputText)
{
    if (currentAction_.isNull())
    {
        return;
    }

    QString trimmedInputText = inputText.trimmed();
    Action* currAction = ActionManager::getInstance().getActionById(currentAction_);
    Action* adoptedAction = nullptr;
    if (!currAction->adopt.isEmpty())
    {
        adoptedAction = ActionManager::getInstance().getActionById(currAction->adopt);
    }
    global::suggestionListController->clearList();
    if (trimmedInputText.isEmpty())
    {
        global::suggestionListController->setVisible(false);
        return;
    }
    
    suggestLinkedActions(currAction, adoptedAction, trimmedInputText);

    if (!currAction->customUiPath.isEmpty())
    {
        auto* customUiItem = this->ui->customUi->rootObject();
        auto* userUi = dynamic_cast<QQuickItem*>(customUiItem->children()[0]);
        QMetaObject::invokeMethod(userUi, "onInputTextChanged", Q_ARG(QVariant, trimmedInputText));
    }

    // plugin provided suggestions
    Action* action = adoptedAction ? adoptedAction : currAction;
    auto* controller = action->controller;
    if (controller)
    {
        auto id = adoptedAction ? adoptedAction->id : currentAction_;
        auto suggestions = controller->getSuggestionItems(id, trimmedInputText);
        for (auto& sugg : suggestions)
        {
            QString completeText = sugg.completeText;
            auto handler = [this, action, controller, completeText](
                SuggestionListController::SuggestionRunType type, boost::any data) -> int {
                size_t token = boost::any_cast<size_t>(data);
                if (!completeText.isEmpty())
                {
                    if (type == SuggestionListController::SuggestionRunType::Enter)
                    {
                        return action->run(completeText);
                    }
                    else
                    {
                        auto* textInput = ui->inputContainer->rootObject();
                        textInput->setProperty("text", completeText);
                        return PG_BEHAVIOR_ON_RETURN::PG_REMAIN;
                    }
                }
                return controller->runSuggestion(token);
            };
            global::suggestionListController->addItem(sugg.text, sugg.imagePath, handler, sugg.token);
        }
    }

    bool visible = global::suggestionListController->getCount() != 0;
    global::suggestionListController->setVisible(visible);
}

void LauncherWindow::suggestLinkedActions(Action* currAction, Action* adoptedAction, const QString& input)
{
    auto emptyRange = decltype(currAction->links){};
    auto joinedLinks = boost::join(currAction->links,
        adoptedAction ? adoptedAction->links : emptyRange);
    for (auto& l : joinedLinks)
    {
        if (l.keyword.startsWith(input, Qt::CaseInsensitive))
        {
            Action* linkedAction = ActionManager::getInstance().getActionById(l.linkedActionId);
            if (!linkedAction)
            {
                continue;
            }

            QString s = QString("<h4>Move to <font color='chocolate'><strong>%1</strong></font> Action</h4><font color='gray'>matched by '<b>%2</b>%3'</font>")
                .arg(linkedAction->name).arg(input).arg(l.keyword.mid(input.length()));
            auto handler = [this, l, linkedAction](SuggestionListController::SuggestionRunType type, boost::any) -> int {
                if (type == SuggestionListController::SuggestionRunType::Tab)
                {
                    auto* textInput = ui->inputContainer->rootObject();
                    textInput->setProperty("text", "");
                    changeAction(l.linkedActionId, l.inputText);
                }
                else if (type == SuggestionListController::SuggestionRunType::Enter)
                {
                    linkedAction->run("");
                }
                return PG_BEHAVIOR_ON_RETURN::PG_REMAIN;
            };
            global::suggestionListController->addItem(s, QString("heart.png"), handler, nullptr);
        }
    }
}

bool LauncherWindow::onKeyPressed(int key, int modifiers, const QString& inputText)
{
    // undo
    if (key == Qt::Key::Key_Z && (modifiers & Qt::Modifier::CTRL) != 0)
    {
        recallInputHistory();
        return false;
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
                return true;
            }
            auto action = ActionManager::getInstance().getActionById(currentAction_);
            int ret = action->run(inputText);

            saveInputHistory(inputText);

            if (ret == PG_BEHAVIOR_ON_RETURN::PG_DISAPPEAR)
            {
                pushDown();
            }
        }
        else
        {
            int ret = global::suggestionListController->runSelected(
                SuggestionListController::SuggestionRunType::Enter);
            if (ret == PG_BEHAVIOR_ON_RETURN::PG_DISAPPEAR)
            {
                pushDown();
            }
            global::suggestionListController->setVisible(false);
        }
    }
    else if (key == Qt::Key::Key_Tab)
    {
        int currentIndex = global::suggestionListController->getCurrentIndex();
        if (currentIndex != -1)
        {
            global::suggestionListController->runSelected(
                SuggestionListController::SuggestionRunType::Tab);
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
    return true;
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
    if (next(actionHistoryPointer_) != actionHistory_.end())
    {
        actionHistoryPointer_++;
        changeAction(*actionHistoryPointer_);
    }
}

void LauncherWindow::saveInputHistory(const QString& inputText)
{
    if (!ui->inputHistoryShowButton->isEnabled())
    {
        ui->inputHistoryShowButton->setEnabled(true);
    }
    inputHistory_.push_back(inputText);
    if (inputHistory_.size() > 10)
    {
        inputHistory_.pop_front();
    }
    inputHistoryIterator_ = inputHistory_.cend();
}

void LauncherWindow::recallInputHistory()
{
    if (!inputHistory_.empty())
    {
        if (inputHistoryIterator_ != inputHistory_.cbegin())
        {
            inputHistoryIterator_--;
        }
        auto* textInput = ui->inputContainer->rootObject();
        textInput->setProperty("text", *inputHistoryIterator_);
    }
}

void LauncherWindow::moveEvent(QMoveEvent *event)
{
    updateSuggestionListPosition();
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
            suggestionBox_->close();
            appExit_ = true;
            close();
        }
        else if (pMsg->wParam == kHotKey_ConfigId_)
        {
            ConfigurationWindow config;
            config.exec();
        }
#   endif // NDEBUG
    }
#   endif // Q_OS_WIN

    return QMainWindow::nativeEvent(eventType, message, result);
}