#include "configmanager.h"
#include "global.h"
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QDebug>
#include <memory>

using namespace std;

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

    //
    settingFile.seek(0);
    QTextStream ts(&settingFile);
    doc.save(ts, 2);
    settingFile.resize(settingFile.pos());
}