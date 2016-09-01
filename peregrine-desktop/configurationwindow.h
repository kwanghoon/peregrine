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
    Q_INVOKABLE bool registerAsStarupApp();
    Q_INVOKABLE bool deregisterAsStarupApp();
    Q_INVOKABLE QVariantMap getConfigs();
    Q_INVOKABLE void setFieldByLocalPath(QObject* target, QString field, QString localPath);
    Q_INVOKABLE void saveActionSlotSettings(const QVariantMap& slotSettings);
    Q_INVOKABLE bool installPlugin(QUrl fileUrl);
};

class ConfigurationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationWindow(QWidget *parent = 0);
    ~ConfigurationWindow();

private:
    Ui::ConfigurationWindow *ui;
};

#endif // CONFIGURATIONWINDOW_H
