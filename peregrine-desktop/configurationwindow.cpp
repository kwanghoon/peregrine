#include "configurationwindow.h"
#include "ui_configurationwindow.h"
#include "configmanager.h"
#include "action.h"
#include "global.h"
#include <utils/StartupRegister.h>
#include <QQuickItem>
#include <QQmlContext>

using namespace std;

namespace
{
    const char* kPeregrineTaskName = "PeregrineStartupTask";
}

ConfigurationWindow::ConfigurationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationWindow)
{
    ui->setupUi(this);
    
    // set context
    auto controller = new ConfigurationController();
    auto context = ui->configurationQuickWidget->rootContext();
    context->setContextProperty("controller", controller);

    ui->configurationQuickWidget->setSource(QUrl::fromLocalFile("ConfigurationForm.ui.qml"));
}

ConfigurationWindow::~ConfigurationWindow()
{
    delete ui;
}

bool ConfigurationController::registerAsStarupApp()
{
    bool ret = utils::registerAsStartupApp(QCoreApplication::applicationFilePath(), kPeregrineTaskName);
    return ret;
}

bool ConfigurationController::deregisterAsStarupApp()
{
    bool ret = utils::deregisterAsStartupApp(kPeregrineTaskName);
    return ret;
}

QVariantMap ConfigurationController::getConfigs()
{
    QVariantMap configs;
    {
        bool registered = utils::isRegisteredAsStartupApp(QCoreApplication::applicationFilePath(), kPeregrineTaskName);
        configs.insert("isStartupApp", registered);

        {
            QVariantList actionList;
            auto& al = ActionManager::getInstance().getActionList();
            for (auto& a : al)
            {
                QVariantMap actionDesc;
                actionDesc.insert("imagePath", a->imagePath);
                actionDesc.insert("actionid", a->id);
                actionList.push_back(actionDesc);
            }
            configs.insert("actionList", actionList);
        }

        // 
        {
            QVariantList slotList;
            for (auto& slot : global::userConfig.actionSlotAssignData)
            {
                QVariantMap slotInfo;
                {
                    slotInfo.insert("x", slot.pos.x());
                    slotInfo.insert("y", slot.pos.y());
                    slotInfo.insert("actionid", slot.actionId);
                }
                slotList.push_back(move(slotInfo));
            }
            configs.insert("slots", slotList);
        }
    }
    return configs;
}

void ConfigurationController::setFieldByLocalPath(QObject* target, QString field, QString localPath)
{
    target->setProperty(field.toStdString().c_str(), QUrl::fromLocalFile(localPath));
}

void ConfigurationController::saveActionSlotSettings(const QVariantMap& slotSettings)
{
    global::GetConfigManager().updateConfig(slotSettings);
}
