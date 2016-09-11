#include "configmanager.h"
#include "syncmanager.h" // #HACK
#include "global.h"
#include <SimpleCrypt/simplecrypt.h>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <memory>

using namespace std;

const uint64_t kAccountCryptKey = (0x20de01f4ec48a1b8 ^ 0x9a70877ba78acb38 + 0x40edfedc7e70bb1a ^ 0x54c13e537e530ce4);

ConfigManager::ConfigManager()
{
}

void ConfigManager::loadConfig()
{
    QFile settingFile("settings.xml");
    QDomDocument doc;
    if (!doc.setContent(&settingFile))
    {
        throw std::runtime_error("Parsing failed. (settings.xml)");
    }
    auto root = doc.documentElement();

    auto pluginElem = root.firstChildElement("plugin");
    global::userConfig.pluginDir = pluginElem.attribute("plugindir");

    auto actionSlotElem = root.firstChildElement("actionslots");
    auto child = actionSlotElem.firstChildElement("actionslot");
    global::userConfig.actionSlotAssignData.clear();
    while (!child.isNull())
    {
        UserConfig::ActionSlotAssignInfo slot;
        {
            slot.actionId = child.attribute("actionid");
            slot.pos = QPoint(child.attribute("x").toInt(),
                child.attribute("y").toInt());
        }
        global::userConfig.actionSlotAssignData.push_back(slot);
        child = child.nextSiblingElement();
    }

    // read account
    auto accountElem = root.firstChildElement("account");
    if (!accountElem.isNull())
    {
        SimpleCrypt crypter;
        crypter.setKey(kAccountCryptKey);
        QString jsonStr = crypter.decryptToString(accountElem.text());
        auto json = QJsonDocument::fromJson(jsonStr.toLocal8Bit());
        account_.filled = true;
        account_.email = json.object()["email"].toString();
        account_.passwordHash = json.object()["password"].toString();
        account_.passwordLength = json.object()["passwordLength"].toInt();
    }

    onConfigUpdated();
}

void ConfigManager::updateConfig(const QVariantMap& config)
{
    QFile settingFile("settings.xml");
    settingFile.open(QIODevice::ReadWrite);
    QDomDocument doc;
    if (!doc.setContent(&settingFile))
    {
        throw std::runtime_error("Parsing failed. (settings.xml)");
    }
    auto root = doc.documentElement();

    // update 
    auto actionSlotsElem = doc.createElement("actionslots");
    const QVariantList& actionSlots = config["slots"].toList();
    for (const auto& e : actionSlots)
    {
        auto& slotSetting = e.toMap();
        QDomElement actionSlotElem = doc.createElement("actionslot");
        {
            int x = slotSetting["x"].toInt();
            actionSlotElem.setAttribute("x", x);
            int y = slotSetting["y"].toInt();
            actionSlotElem.setAttribute("y", y);
            QString id = slotSetting["actionId"].toString();
            actionSlotElem.setAttribute("actionid", id);
        }
        actionSlotsElem.appendChild(actionSlotElem);
    }
    root.replaceChild(actionSlotsElem, root.firstChildElement("actionslots"));

    // save
    settingFile.seek(0);
    QTextStream ts(&settingFile);
    doc.save(ts, 2);
    settingFile.resize(settingFile.pos());
    settingFile.close();

    loadConfig();

    onConfigUpdated();

    // #HACK:
    global::GetSyncManager().putConfigs(config);
}

void ConfigManager::updateAccountConfig(const QVariantMap& accountConfig)
{
    QFile settingFile("settings.xml");
    settingFile.open(QIODevice::ReadWrite);
    QDomDocument doc;
    if (!doc.setContent(&settingFile))
    {
        throw std::runtime_error("Parsing failed. (settings.xml)");
    }
    auto root = doc.documentElement();

    // update
    // account information is stored with encryption for security.
    auto accountElem = doc.createElement("account");
    QString json = QJsonDocument::fromVariant(accountConfig).toJson();
    SimpleCrypt crypter;
    crypter.setKey(kAccountCryptKey);
    auto textNode = doc.createTextNode(crypter.encryptToString(json));
    accountElem.appendChild(textNode);

    auto old = root.firstChildElement("account");
    if (!old.isNull())
    {
        root.removeChild(old);
    }
    root.appendChild(accountElem);

    // save
    settingFile.seek(0);
    QTextStream ts(&settingFile);
    doc.save(ts, 2);
    settingFile.resize(settingFile.pos());
    settingFile.close();
}

const ConfigManager::AccountInfo& ConfigManager::getAccountInfo() const
{
    return account_;
}
