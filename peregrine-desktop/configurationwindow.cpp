#include "configurationwindow.h"
#include "ui_configurationwindow.h"
#include <QQuickItem>
#include <QQmlContext>
#include "../utils/StartupRegister.h"

namespace
{
    const char* kPeregrineTaskName = "PeregrineStartupTask";
}

ConfigurationWindow::ConfigurationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationWindow)
{
    ui->setupUi(this);
    ui->configurationQuickWidget->setSource(QUrl::fromLocalFile("ConfigurationForm.ui.qml"));

    // set context
    auto controller = new ConfigurationController();
    auto context = ui->configurationQuickWidget->rootContext();
    context->setContextProperty("controller", controller);

    QQuickItem* root = ui->configurationQuickWidget->rootObject();

    QVariantMap configs;
    bool registered = utils::isRegisteredAsStartupApp(QCoreApplication::applicationFilePath(), kPeregrineTaskName);
    configs.insert("isStartupApp", registered);
    QMetaObject::invokeMethod(root, "initConfigs", Q_ARG(QVariant, QVariant::fromValue(configs)));
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