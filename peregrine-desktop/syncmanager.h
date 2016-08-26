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

    void login(const QString& id, const QString& password, 
        std::function<void()> thenFunc = {}, std::function<void()> catchFunc = {});
    void putConfigs(const QVariantMap& configs);
    void getConfigs(std::function<void(const QVariantMap& configs)> thenFunc = {}, std::function<void()> catchFunc = {});

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