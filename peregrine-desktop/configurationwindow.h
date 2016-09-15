#ifndef CONFIGURATIONWINDOW_H
#define CONFIGURATIONWINDOW_H

#include <QDialog>
#include <QVariantMap>
#include <QUrl>

namespace Ui {
class ConfigurationWindow;
}

class ConfigurationController : public QObject
{
    Q_OBJECT

public:
    // #HACK: QT documents instructed to specify a NOTIFY signal for QML use.
    //        but, it seems to work despite of the lack of it.
    Q_PROPERTY(qreal initialTabIndex MEMBER initialTabIndex);
    Q_INVOKABLE bool registerAsStarupApp();
    Q_INVOKABLE bool deregisterAsStarupApp();
    Q_INVOKABLE QVariantMap getConfigs();
    Q_INVOKABLE void setFieldByLocalPath(QObject* target, QString field, QString localPath);
    Q_INVOKABLE void saveActionSlotSettings(const QVariantMap& slotSettings);
    Q_INVOKABLE bool installPlugin(QUrl fileUrl);
    Q_INVOKABLE bool login(const QString& email, const QString& password);

    qreal initialTabIndex = 0;
};

class ConfigurationWindow : public QDialog
{
    Q_OBJECT

public:
    enum class TabKind
    {
        Default,
        Account,
    };

    explicit ConfigurationWindow(QWidget *parent = 0, TabKind tab = TabKind::Default);
    ~ConfigurationWindow();

private:
    Ui::ConfigurationWindow *ui;
};

#endif // CONFIGURATIONWINDOW_H
