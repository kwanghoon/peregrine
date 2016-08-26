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
    void putConfigs(const QVariantMap& configs);
    void getConfigs();

private:
    QNetworkAccessManager* networkManager_ = nullptr;
    enum class State
    {
        unconnected,
        connectionFailed,
        loginFailed,
        connected,
    };
    State state_ = State::unconnected;
    QString id_;
};