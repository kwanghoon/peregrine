// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#pragma once

#include <QString>
#include <QObject>
#include <functional>

class QNetworkAccessManager;
class QNetworkReply;

class SyncManager : public QObject
{
    Q_OBJECT

public:
    SyncManager();

    void login(const QString& id, const QString& passwordHash, 
        std::function<void(const QJsonObject& json)> thenFunc = {}, std::function<void()> catchFunc = {});
    void logout();
    void putConfigs(const QVariantMap& configs);
    void getConfigs(std::function<void(const QJsonObject& configs)> thenFunc = {}, std::function<void()> catchFunc = {});
    bool isLogined() const { return !email_.isEmpty(); }

private:
    void sendGetRequest(const QString& path, std::function<void(const QJsonObject& json)> thenFunc, std::function<void()> catchFunc = {});
    void sendPostRequest(const QString& path, const QVariantMap& formData, std::function<void()> thenFunc, std::function<void()> catchFunc = {});

    QNetworkAccessManager* networkManager_ = nullptr;
    QString email_;
    QString passwordHash_;
};