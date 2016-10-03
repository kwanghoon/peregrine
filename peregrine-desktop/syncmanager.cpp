#include "syncmanager.h"
#include "configmanager.h"
#include <global.h>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <cassert>

using namespace std;

SyncManager::SyncManager()
{
    networkManager_ = new QNetworkAccessManager(this);
    connect(&global::GetConfigManager(), &ConfigManager::onConfigUpdated,
        [this](const QVariantMap& config, const QString& reason) {
            if (reason != "sync")
            {
                putConfigs(config);
            }
        });
}

void SyncManager::login(const QString& email, const QString& passwordHash,
    function<void(const QJsonObject& json)> thenFunc, function<void()> catchFunc)
{
    email_ = email;
    passwordHash_ = passwordHash;
    getConfigs([thenFunc](const QJsonObject& json) {
            if (thenFunc)
            {
                thenFunc(json["config"].toObject());
            }
        },
        [this, &catchFunc]() {
            email_.clear();
            passwordHash_.clear();
            catchFunc();
        });
}

void SyncManager::putConfigs(const QVariantMap& configs)
{
    if (email_.isEmpty())
    {
        return;
    }
    QVariantMap data{ {"config", configs} };
    sendPostRequest("/config", move(data), {}, {});
}

void SyncManager::getConfigs(function<void(const QJsonObject& json)> thenFunc, function<void()> catchFunc)
{
    if (email_.isEmpty())
    {
        return;
    }
    QUrlQuery query;
    query.setQueryItems({ {"email", email_}, {"pwHash", passwordHash_} });
    sendGetRequest("/config", 
        [thenFunc](const QJsonObject& json) {
            if (thenFunc)
            {
                thenFunc(json);
            }
        });
}

void SyncManager::sendGetRequest(const QString& path,
    function<void(const QJsonObject& json)> thenFunc, function<void()> catchFunc)
{
    QUrl serverUrl = global::GetConfigManager().getSyncServerUrl();
    serverUrl.setPath(path);

    QUrlQuery query;
    query.setQueryItems({ { "email", email_ },{ "pwHash", passwordHash_ } });
    serverUrl.setQuery(query);

    QNetworkRequest req(serverUrl);
    QNetworkReply* reply = networkManager_->get(req);
    connect(reply, &QNetworkReply::finished, [this, reply, thenFunc, catchFunc]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            if (catchFunc) 
            {
                catchFunc();
            };
            return;
        }
        auto content = reply->readAll();
        auto json = QJsonDocument::fromJson(content);
        assert(!json.isNull());
        //qDebug() << json.toJson(QJsonDocument::Indented);
        auto obj = json.object();
        if (obj["success"].toBool() == true)
        {
            if (thenFunc) 
            {
                thenFunc(obj);
            };
        }
        else
        {
            if (catchFunc)
            {
                catchFunc();
            };
        }
    });
}

void SyncManager::sendPostRequest(const QString& path, const QVariantMap& formData,
    function<void()> thenFunc, function<void()> catchFunc)
{
    QUrl serverUrl = global::GetConfigManager().getSyncServerUrl();
    serverUrl.setPath(path);

    QNetworkRequest req(serverUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto json = QJsonDocument::fromVariant(formData).object();
    json.insert("email", email_);
    json.insert("pwHash", passwordHash_);
    QNetworkReply* reply = networkManager_->post(req, QJsonDocument(json).toJson());
    connect(reply, &QNetworkReply::finished, [this, reply, thenFunc, catchFunc]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            if (catchFunc)
            {
                catchFunc();
            };
            return;
        }
        auto content = reply->readAll();
        auto json = QJsonDocument::fromJson(content);
        assert(!json.isNull());
        //qDebug() << json.toJson(QJsonDocument::Indented);
        auto obj = json.object();
        if (obj["success"].toBool() == true)
        {
            if (thenFunc)
            {
                thenFunc();
            };
        }
        else
        {
            if (catchFunc)
            {
                catchFunc();
            };
        }
    });
}