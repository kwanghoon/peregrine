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
    Q_INVOKABLE void onInputTextChanged(const QString& inputText);
    Q_INVOKABLE void onSuggestionItemClicked(int index);
    Q_INVOKABLE void onKeyPressed(int key, const QString& inputText);

public slots:
};

#endif // INPUTHANDLERDELEGATE_H
