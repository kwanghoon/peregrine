#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QUrl>

class QDomElement;

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
    const QString& getStartAction() const { return startAction_; }
    const QUrl& getSyncServerUrl() const { return syncServerUrl_; }

    int getSlots() const
    {
        return receivers(SIGNAL(ConfigManager::onConfigUpdated()));
    }

signals:
    void onConfigUpdated(const QVariantMap& config, const QString& reason);
    void onAccountInfoUpdated();

private:
    void readSyncSettings(QDomElement& root);

    AccountInfo account_;
    int maxSuggestions_;
    QString startAction_;
    QUrl syncServerUrl_;
};

#endif // CONFIGMANAGER_H
