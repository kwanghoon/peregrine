#include "suggestionlistcontroller.h"
#include "inputhandlerdelegate.h"
#include <QQmlProperty>
#include <QVariant>
#include <QObject>
#include <cassert>

SuggestionListController::SuggestionListController(QQuickItem* suggestionListView, 
    QObject* suggestionModel, InputHandlerDelegate* inputDelegate)
    : suggestionListView_(suggestionListView)
    , suggestionModel_(suggestionModel)
    , inputDelegate_(inputDelegate)
{
    QObject::connect(inputDelegate_, &InputHandlerDelegate::onSuggestionItemClicked,
        [this](int index) { onSuggestionItemClicked(index); });
}

void SuggestionListController::addItem(const QString& text, std::function<void(boost::any)> handler, boost::any data)
{
    QVariant arg(text);
    QMetaObject::invokeMethod(suggestionModel_, "add",
        Q_ARG(QVariant, arg));
    SuggestingItem item;
    {
        item.handler = handler;
        item.data = data;
    }
    suggestingItems_.push_back(item);
}

void SuggestionListController::clearList()
{
    QMetaObject::invokeMethod(suggestionModel_, "clear");
    suggestingItems_.clear();
}

void SuggestionListController::selectUp()
{
    QMetaObject::invokeMethod(suggestionListView_, "moveSelectUp");
}

void SuggestionListController::selectDown()
{
    QMetaObject::invokeMethod(suggestionListView_, "moveSelectDown");
}

int SuggestionListController::getCurrentIndex() const
{
    int currentIndex = QQmlProperty::read(suggestionListView_, "currentIndex").toInt();
    return currentIndex;
}

void SuggestionListController::runSelected()
{
    QMetaObject::invokeMethod(suggestionListView_, "runSelected");
}

void SuggestionListController::onSuggestionItemClicked(int index)
{
    suggestingItems_[index].handler(suggestingItems_[index].data);
}
