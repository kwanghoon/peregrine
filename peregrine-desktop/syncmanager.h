#pragma once

#include <QString>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class SyncManager : public QObject
{
    Q_OBJECT

public:
    SyncManager();

    void login(const QString& id, const QString& password);

private:
    void finished(QNetworkReply* reply);

    QNetworkAccessManager* networkManager_ = nullptr;
};