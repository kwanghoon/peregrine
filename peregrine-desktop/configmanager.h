// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

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
