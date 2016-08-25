#include "configmanager.h"
#include "global.h"
#include <QXmlSimpleReader>
#include <QDebug>
#include <memory>

using namespace std;

ConfigManager::ConfigManager()
{
}

void ConfigManager::loadConfig()
{
    struct SettingXMLContentHandler : QXmlDefaultHandler
    {
        SettingXMLContentHandler() : QXmlDefaultHandler() {}

        bool startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts) override
        {
            if (localName == "plugin")
            {
                global::userConfig.pluginDir = atts.value("plugindir");
                qDebug() << global::userConfig.pluginDir;
            }
            else if (localName == "actionslot")
            {
                UserConfig::ActionSlotAssignInfo slot;
                {
                    slot.actionId = atts.value("actionid");
                    slot.pos = QPoint(atts.value("x").toInt(),
                        atts.value("y").toInt());
                }
                global::userConfig.actionSlotAssignData.push_back(slot);

            }
            return true;
        }
    };
    QXmlSimpleReader xmlReader;
    QFile settingFile("settings.xml");
    unique_ptr<QXmlInputSource> source(new QXmlInputSource(&settingFile));
    unique_ptr<SettingXMLContentHandler> contentHandler(new SettingXMLContentHandler);
    xmlReader.setContentHandler(contentHandler.get());
    if (!xmlReader.parse(source.get()))
    {
        throw std::runtime_error("Parsing failed. (settings.xml)");
    }
}
