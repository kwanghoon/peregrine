// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

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
#include <QMessageBox>
#include <QJsonObject>

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
    else if (tab == TabKind::About)
    {
        controller->initialTabIndex = 4;
    }

    ui->configurationQuickWidget->setSource(QUrl::fromLocalFile("ConfigurationForm.ui.qml"));
}

ConfigurationWindow::~ConfigurationWindow()
{
    delete ui;
}

bool ConfigurationController::registerAsStartupApp()
{
    bool ret = utils::registerAsStartupApp(QCoreApplication::applicationFilePath(), kPeregrineTaskName);
    return ret;
}

bool ConfigurationController::deregisterAsStartupApp()
{
    bool ret = utils::deregisterAsStartupApp(kPeregrineTaskName);
    return ret;
}

QVariantMap ConfigurationController::getConfigs()
{
    QVariantMap configs;
    {
        // General
        bool registered = utils::isRegisteredAsStartupApp(
            QCoreApplication::applicationFilePath(), kPeregrineTaskName);
        configs.insert("isStartupApp", registered);
        configs.insert("maxSuggestions", global::GetConfigManager().getMaxSuggestions());

        // Actions
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
            configs.insert("actionslots", slotList);
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

        // About
        QString versionInfo = QString("%1 %2").arg(global::getAppVersion())
#       ifdef NDEBUG
            .arg("Release");
#       else
            .arg("Debug");
#       endif
        configs.insert("version", versionInfo);
    }
    return configs;
}

void ConfigurationController::setFieldByLocalPath(QObject* target, QString field, QString localPath)
{
    target->setProperty(field.toStdString().c_str(), QUrl::fromLocalFile(localPath));
}

void ConfigurationController::saveConfigs(const QVariantMap& configs)
{
    global::GetConfigManager().updateConfig(configs, "modify");
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
    auto thenFunc = [accountInfo](const QJsonObject& json) {
        
        // first, save the account information
        QVariantMap config;
        {
            config.insert("sync", QVariantMap { { "account", accountInfo } });
        }
        global::GetConfigManager().updateConfig(config, "modify");

        // ask user whether to up-sync or down-sync
        QMessageBox box;
        box.setText("Would you like to down-sync configurations? or up-sync the current configurations?");
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        int answer = box.exec();
        if (answer == QMessageBox::Yes)
        {
            auto thenFunc = [](const QJsonObject& configs) {
                global::GetConfigManager().updateConfig(configs.toVariantMap()["config"].toMap(), "sync");
            };
            global::GetSyncManager().getConfigs(thenFunc);
        }
        else
        {}
    };
    auto catchFunc = []() {
        QMessageBox* box = new QMessageBox();
        box->setText("Failed.");
        box->show();
        QObject::connect(box, &QMessageBox::finished, [box]() {
            box->deleteLater();
        });
    };
    global::GetSyncManager().login(email, passwordHash, thenFunc, catchFunc);
    return true;
}

bool ConfigurationController::logout()
{
    QVariantMap config;
    {
        config.insert("sync", QVariantMap{ { "account", {} } });
    }
    global::GetConfigManager().updateConfig(config, "modify");
    global::GetSyncManager().logout();
    return true;
}
