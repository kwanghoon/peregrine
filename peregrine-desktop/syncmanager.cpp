#include "syncmanager.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrlQuery>
#include <QDebug>

SyncManager::SyncManager()
{
    networkManager_ = new QNetworkAccessManager();
    QObject::connect(networkManager_, &QNetworkAccessManager::finished, this, &SyncManager::finished);
}

void SyncManager::login(const QString& id, const QString& password)
{
    QUrl serverUrl("http://127.0.0.1/login/");
    QUrlQuery loginQuery;
    loginQuery.setQueryItems({{"id", id}, {"pw", password}});
    serverUrl.setQuery(loginQuery);
    serverUrl.setPort(8000);
    QNetworkRequest loginRequest(serverUrl);
    networkManager_->get(loginRequest);
}

void SyncManager::finished(QNetworkReply* reply)
{
    qDebug() << reply->readAll().toStdString().c_str();
}
