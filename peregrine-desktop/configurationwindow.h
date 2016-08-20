#ifndef CONFIGURATIONWINDOW_H
#define CONFIGURATIONWINDOW_H

#include <QDialog>

namespace Ui {
class ConfigurationWindow;
}

class ConfigurationController : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE bool registerAsStarupApp();
    Q_INVOKABLE bool deregisterAsStarupApp();
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
