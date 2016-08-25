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