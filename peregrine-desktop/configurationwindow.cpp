#include "configurationwindow.h"
#include "ui_configurationwindow.h"
#include "configmanager.h"
#include "syncmanager.h"
#include "action.h"
#include "global.h"
#include "plugin.h"
#include <utils/StartupRegister.h>
#include <quazip.h>
#include <quazipfile.h>
#include <QQuickItem>
#include <QQmlContext>
#include <QCryptographicHash>

using namespace std;

namespace
{
    const char* kPeregrineTaskName = "PeregrineStartupTask";
}

ConfigurationWindow::ConfigurationWindow(QWidget *parent, TabKind tab) :
    QDialog(parent),
    ui(new Ui::ConfigurationWindow)
{
    ui->setupUi(this);
    
    // set context
    auto controller = new ConfigurationController();
    auto context = ui->configurationQuickWidget->rootContext();
    context->setContextProperty("controller", controller);
    if (tab == TabKind::Account)
    {
        controller->initialTabIndex = 3;
    }

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

        //
        auto& accountInfo = global::GetConfigManager().getAccountInfo();
        if (accountInfo.filled)
        {
            QVariantMap v;
            v.insert("email", accountInfo.email);
            v.insert("passwordLength", accountInfo.passwordLength);
            configs.insert("account", v);
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
    global::GetConfigManager().updateConfig(slotSettings, "modify");
}

bool ConfigurationController::installPlugin(QUrl fileUrl)
{
    // checks if file extension matches *.wing
    QFileInfo fileInfo = fileUrl.toLocalFile();
    if (fileInfo.suffix() != "wing")
    {
        return false;
    }

    if (!fileInfo.exists())
    {
        return false;
    }

    QuaZip pluginZip(fileInfo.absoluteFilePath());
    if (!pluginZip.open(QuaZip::mdUnzip))
    {
        return false;
    }

    QStringList fileList = pluginZip.getFileNameList();

    if (fileList.filter("plugin.xml").isEmpty())
    {
        return false;
    }

    // clean up temp dir
    QDir tempDir;
    if (tempDir.cd("temp-plugin"))
    {
        tempDir.removeRecursively();
        tempDir.cdUp();
    }
    tempDir.mkdir("temp-plugin");
    tempDir.cd("temp-plugin");

    // unzip
    pluginZip.goToFirstFile();
    for (QString f : fileList)
    {
        QuaZipFile archived(&pluginZip);
        archived.open(QIODevice::ReadOnly);

        auto uncompressed = archived.readAll();
        QFile created(tempDir.absoluteFilePath(f));
        created.open(QFile::WriteOnly);
        created.write(uncompressed);
        created.close();

        pluginZip.goToNextFile();
    }
    pluginZip.close();

    // rename temporary dir
    QString name = PluginManager::getPluginNameFromDir(tempDir);
    QString newPluginDir("plugins/" + name);
    if (!QDir().rename(tempDir.path(), newPluginDir))
    {
        tempDir.removeRecursively();
        return false;
    }

    // load
    PluginManager::getInstance().loadPlugin(newPluginDir);

    return true;
}

bool ConfigurationController::login(const QString& email, const QString& password)
{
    QString salted = "zelkova-" + password + 'x' + email + "-peregrine";
    QString passwordHash = QCryptographicHash::hash(salted.toUtf8(),
        QCryptographicHash::Sha256).toBase64();
    QVariantMap accountInfo;
    {
        accountInfo["email"] = email;
        accountInfo["password"] = passwordHash;
        accountInfo["salt"] = "peregrine-the-crossplatform-launcher";
        accountInfo["passwordLength"] = password.length();
    }
    global::GetSyncManager().login(email, passwordHash);
    global::GetConfigManager().updateAccountConfig(accountInfo);
    return true;
}
