#ifndef INPUTHANDLERDELEGATE_H
#define INPUTHANDLERDELEGATE_H

#include <QObject>

class InputHandlerDelegate : public QObject
{
    Q_OBJECT

public:
    explicit InputHandlerDelegate(QObject *parent = 0);

    Q_INVOKABLE void onInputTextAccepted(const QString& inputText);

signals:

public slots:
};

#endif // INPUTHANDLERDELEGATE_H
