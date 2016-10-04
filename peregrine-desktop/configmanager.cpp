#include "configmanager.h"
#include "global.h"
#include "action.h"
#include <SimpleCrypt/simplecrypt.h>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <memory>

using namespace std;

const uint64_t kAccountCryptKey = (0x20de01f4ec48a1b8 ^ 0x9a70877ba78acb38 + 0x40edfedc7e70bb1a ^ 0x54c13e537e530ce4);

ConfigManager::ConfigManager()
{}

void ConfigManager::loadConfig()
{
    QFile settingFile("settings.xml");
    QDomDocument doc;
    if (!doc.setContent(&settingFile))
    {
        throw std::runtime_error("Parsing failed. (settings.xml)");
    }
    auto root = doc.documentElement();

    startAction_ = root.attribute("startAction");

    auto pluginElem = root.firstChildElement("plugin");
    global::userConfig.pluginDir = pluginElem.attribute("plugindir");


    auto elem = root.firstChildElement("maxSuggestions");
    Q_ASSERT(!elem.isNull());
    maxSuggestions_ = elem.text().toInt();

    
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

    // read custom actions
    auto customActionsElem = root.firstChildElement("customactions");
    if (!customActionsElem.isNull())
    {
        for (auto action = customActionsElem.firstChildElement("action");
            !action.isNull(); action = action.nextSiblingElement("action"))
        {
            LoadAction(action, QDir());
        }
    }

    auto syncElem = root.firstChildElement("sync");
    if (!syncElem.isNull())
    {
        readSyncSettings(syncElem);
    }
}

void ConfigManager::readSyncSettings(QDomElement& syncElem)
{
    auto serverUrl = syncElem.firstChildElement("serverUrl");
    if (!serverUrl.isNull())
    {
        QString ip = serverUrl.attribute("ip");
        QString port = serverUrl.attribute("port");
        syncServerUrl_.setScheme("http");
        syncServerUrl_.setHost(ip);
        syncServerUrl_.setPort(port.toInt());
    }

    auto accountElem = syncElem.firstChildElement("account");
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
        onAccountInfoUpdated();
    }
}

void ConfigManager::updateConfig(const QVariantMap& config, const QString& reason)
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
    if (config.contains("actionslots"))
    {
        auto actionSlotsElem = doc.createElement("actionslots");
        const QVariantList& actionSlots = config["actionslots"].toList();
        for (const auto& e : actionSlots)
        {
            auto slotSetting = e.toMap();
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
    }
    else if (config.contains("maxSuggestions"))
    {
        auto elem = root.firstChildElement("maxSuggestions");
        Q_ASSERT(!elem.isNull());
        QDomText text = elem.firstChild().toText();
        text.setNodeValue(config["maxSuggestions"].toString());
    }
    // account information is stored with encryption for security.
    else if (config.contains("sync"))
    {
        // ensure 'sync' element exists
        auto syncElem = root.firstChildElement("sync");
        if (syncElem.isNull())
        {
            syncElem = doc.createElement("sync");
            root.appendChild(syncElem);
        }
        
        // create a new account element using config
        auto accountElem = doc.createElement("account");
        QString json = QJsonDocument::fromVariant(config["sync"].toMap()["account"]).toJson();
        SimpleCrypt crypter;
        crypter.setKey(kAccountCryptKey);
        auto textNode = doc.createTextNode(crypter.encryptToString(json));
        accountElem.appendChild(textNode);

        // set. 
        auto old = syncElem.firstChildElement("account");
        if (!old.isNull())
        {
            syncElem.removeChild(old);
        }
        syncElem.appendChild(accountElem);
    }

    // save
    settingFile.seek(0);
    QTextStream ts(&settingFile);
    doc.save(ts, 2);
    settingFile.resize(settingFile.pos());
    settingFile.close();

    loadConfig();

    onConfigUpdated(config, reason);
}

const ConfigManager::AccountInfo& ConfigManager::getAccountInfo() const
{
    return account_;
}
