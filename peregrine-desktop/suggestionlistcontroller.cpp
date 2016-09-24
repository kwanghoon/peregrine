#include "suggestionlistcontroller.h"
#include "inputhandlerdelegate.h"
#include <QQmlProperty>
#include <QVariant>
#include <QTimer>
#include <QWidget>
#include <QObject>
#include <cassert>

SuggestionListController::SuggestionListController(QWidget* suggestionListWidget, 
    QQuickItem* suggestionListView,
    QObject* suggestionModel, InputHandlerDelegate* inputDelegate)
    : suggestionListWidget_(suggestionListWidget)
    , suggestionListView_(suggestionListView)
    , suggestionModel_(suggestionModel)
    , inputDelegate_(inputDelegate)
{
    suggestionListWidget_->setVisible(false);

    QObject::connect(inputDelegate_, &InputHandlerDelegate::onSuggestionItemClicked,
        [this](int index) { onSuggestionItemClicked(index); });
}

void SuggestionListController::addItem(const QString& text, const QString& imagePath, 
    std::function<void(boost::any)> handler, boost::any data)
{
    QVariant arg(text);

    QMetaObject::invokeMethod(suggestionListView_, "addItem", 
        Q_ARG(QVariant, arg), Q_ARG(QVariant, imagePath));

    // #HACK: size of the ListView's children isn't updated immediately after adding.
    //        to get the size reflecting new items added, have a small bit of delays.
    const int kUIUpdateWaitDelay = 20;
    QTimer::singleShot(kUIUpdateWaitDelay, [this]() {
        QMetaObject::invokeMethod(suggestionListView_, "fitHeightToChildren");
    });

    SuggestingItem item;
    {
        item.handler = handler;
        item.data = data;
    }
    suggestingItems_.push_back(item);

    suggestionListWidget_->setVisible(true);
}

void SuggestionListController::clearList()
{
    suggestionListWidget_->setVisible(false);

    QMetaObject::invokeMethod(suggestionListView_, "clearItems");

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

void SuggestionListController::setVisible(bool visible)
{
    QQmlProperty::write(suggestionListView_, "visible", visible);
}

void SuggestionListController::onSuggestionItemClicked(int index)
{
    suggestingItems_[index].handler(suggestingItems_[index].data);
}
