#include "configurationwindow.h"
#include "ui_configurationwindow.h"
#include <utils/StartupRegister.h>
#include "action.h"
#include <QQuickItem>
#include <QQmlContext>

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

        QVariantList actionList;
        auto& al = ActionManager::getInstance().getActionList();
        for (auto& a : al)
        {
            QVariantMap actionDesc;
            actionDesc.insert("image", a->imagePath);
            actionDesc.insert("actionid", a->id);
            actionList.push_back(actionDesc);
        }
        configs.insert("actionList", actionList);
    }
    return configs;
}