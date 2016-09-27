#include "suggestionlistcontroller.h"
#include "inputhandlerdelegate.h"
#include <QQmlProperty>
#include <QVariant>
#include <QTimer>
#include <QWidget>
#include <QObject>
#include <cassert>
#include <QQuickWidget>

SuggestionListController::SuggestionListController(QQuickWidget* suggestionBox, 
    QQuickItem* suggestionListView, QObject* suggestionModel, 
    InputHandlerDelegate* inputDelegate)
    : suggestionBox_(suggestionBox)
    , suggestionListView_(suggestionListView)
    , suggestionModel_(suggestionModel)
    , inputDelegate_(inputDelegate)
{
    setVisible(false);
    QObject::connect(inputDelegate_, &InputHandlerDelegate::onSuggestionItemClicked,
        this, &SuggestionListController::onSuggestionItemClicked);
}

void SuggestionListController::addItem(const QString& text, const QString& imagePath, 
    std::function<int(SuggestionRunType, boost::any)> handler, boost::any data)
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
}

void SuggestionListController::clearList()
{
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

int SuggestionListController::runSelected(SuggestionListController::SuggestionRunType type)
{
    QVariant ret;
    QMetaObject::invokeMethod(suggestionListView_, "runSelected",
        Q_RETURN_ARG(QVariant, ret),
        Q_ARG(QVariant, static_cast<int>(type)));
    return ret.toInt();
}

void SuggestionListController::setVisible(bool visible)
{
    suggestionBox_->setVisible(visible);
    if (visible)
    {
        suggestionBox_->raise();
    }
}

int SuggestionListController::onSuggestionItemClicked(int type, int index)
{
    return suggestingItems_[index].handler((SuggestionListController::SuggestionRunType)(type),
        suggestingItems_[index].data);
}
