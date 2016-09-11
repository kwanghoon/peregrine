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
    void updateAccountConfig(const QVariantMap& accountConfig);

    struct AccountInfo
    {
        bool filled = false;
        QString email;
        QString passwordHash;
        int passwordLength;
    };
    const AccountInfo& getAccountInfo() const;

signals:
    void onConfigUpdated();

private:
    AccountInfo account_;
};

#endif // CONFIGMANAGER_H
