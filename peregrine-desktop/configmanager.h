#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>

class ConfigManager : public QObject
{
    Q_OBJECT

public:
    ConfigManager();

    void loadConfig();

    void updateConfig(const QVariantMap& config, const QString& reason);
    void updateAccountConfig(const QVariantMap& accountConfig);

    struct AccountInfo
    {
        bool filled = false;
        QString email;
        QString passwordHash;
        int passwordLength;
    };
    const AccountInfo& getAccountInfo() const;
    int getMaxSuggestions() const { return maxSuggestions_; }

    int getSlots() const
    {
        return receivers(SIGNAL(ConfigManager::onConfigUpdated()));
    }

signals:
    void onConfigUpdated(const QVariantMap& config, const QString& reason);
    void onAccountInfoUpdated();

private:
    AccountInfo account_;
    int maxSuggestions_;
};

#endif // CONFIGMANAGER_H
