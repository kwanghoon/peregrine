#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    ConfigManager();

    void loadConfig();

    void updateConfig(const QVariantMap& config);

signals:
    void onConfigUpdated();
};

#endif // CONFIGMANAGER_H
