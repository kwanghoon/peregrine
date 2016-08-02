#include "suggestionlistcontroller.h"
#include <QVariant>
#include <QObject>
#include <cassert>

SuggestionListController::SuggestionListController(QQuickItem* suggestionListView, QObject* suggestionModel)
    : suggestionListView_(suggestionListView)
    , suggestionModel_(suggestionModel)
{
}

void SuggestionListController::add(const QString& text)
{
    QVariant arg(text);
    QMetaObject::invokeMethod(suggestionModel_, "add",
        Q_ARG(QVariant, arg));
}

void SuggestionListController::clear()
{
    QMetaObject::invokeMethod(suggestionModel_, "clear");
}
