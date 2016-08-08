#ifndef INPUTHANDLERDELEGATE_H
#define INPUTHANDLERDELEGATE_H

#include <QObject>

class QLibrary;

class InputHandlerDelegate : public QObject
{
    Q_OBJECT

public:
    explicit InputHandlerDelegate(QObject *parent = 0);

signals:
    Q_INVOKABLE void onInputTextAccepted(const QString& inputText);
    Q_INVOKABLE void onInputTextChanged(const QString& inputText);
    Q_INVOKABLE void onSuggestionItemClicked(int index);
    Q_INVOKABLE void onShiftKeyPressed();

public slots:
};

#endif // INPUTHANDLERDELEGATE_H
