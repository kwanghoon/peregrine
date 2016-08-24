#ifndef CONFIGURATIONWINDOW_H
#define CONFIGURATIONWINDOW_H

#include <QDialog>
#include <QVariantMap>

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
