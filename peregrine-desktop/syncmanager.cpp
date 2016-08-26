#include "syncmanager.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <cassert>

SyncManager::SyncManager()
{
    networkManager_ = new QNetworkAccessManager(this);
}

void SyncManager::login(const QString& id, const QString& password,
    std::function<void()> thenFunc, std::function<void()> catchFunc)
{
    QUrl serverUrl("http://127.0.0.1/login/");
    QUrlQuery loginQuery;
    loginQuery.setQueryItems({ {"id", id}, {"pw", password} });
    serverUrl.setQuery(loginQuery);
    serverUrl.setPort(8000);
    QNetworkRequest loginRequest(serverUrl);
    QNetworkReply* reply = networkManager_->get(loginRequest);
    connect(reply, &QNetworkReply::finished, [this, reply, thenFunc, catchFunc]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            state_ = State::connectionFailed;
            if (catchFunc) { catchFunc(); };
            return;
        }
        auto content = reply->readAll();
        auto json = QJsonDocument::fromJson(content);
        assert(!json.isNull());
        auto obj = json.object();
        if (obj["success"].toInt() == 1)
        {
            state_ = State::connected;
            if (thenFunc) { thenFunc(); };
        }
        else
        {
            state_ = State::loginFailed;
            if (catchFunc) { catchFunc(); };
        }
    });
    id_ = id;
}

void SyncManager::putConfigs(const QVariantMap& configs)
{
    if (state_ != State::connected)
    {
        return;
    }

    QUrl serverUrl("http://127.0.0.1/configs/");
    QUrlQuery putConfigsQuery;
    putConfigsQuery.setQueryItems({{ "id", id_ }});
    serverUrl.setQuery(putConfigsQuery);
    serverUrl.setPort(8000);
    QNetworkRequest putConfigsRequest(serverUrl);
    putConfigsRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto json = QJsonDocument::fromVariant(configs).toJson();
    QNetworkReply* reply = networkManager_->put(putConfigsRequest, json);
}

void SyncManager::getConfigs(std::function<void(const QVariantMap& configs)> thenFunc, std::function<void()> catchFunc)
{
    QUrl serverUrl("http://127.0.0.1/configs/");
    QUrlQuery getConfigsQuery;
    getConfigsQuery.setQueryItems({ { "id", id_ } });
    serverUrl.setQuery(getConfigsQuery);
    serverUrl.setPort(8000);
    QNetworkRequest putConfigsRequest(serverUrl);
    QNetworkReply* reply = networkManager_->get(putConfigsRequest);
    connect(reply, &QNetworkReply::finished, [this, reply, thenFunc, catchFunc]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            if (catchFunc) { catchFunc(); };
            return;
        }
        auto content = reply->readAll();
        auto json = QJsonDocument::fromJson(content);
        assert(!json.isNull());
        
        auto obj = json.object();
        if (obj["success"].toInt() == 1)
        {
            if (thenFunc) { thenFunc(json.toVariant().toMap()); }
        }
        else
        {
            if (catchFunc) { catchFunc(); };
        }
    });
}
