#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>

class ConfigManager
{
public:
    ConfigManager();

    void loadConfig();

    void updateConfig(const QVariantMap& config);
};

#endif // CONFIGMANAGER_H
