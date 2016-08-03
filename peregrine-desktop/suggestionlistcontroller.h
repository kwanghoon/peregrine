#ifndef SUGGESTIONLISTCONTROLLER_H
#define SUGGESTIONLISTCONTROLLER_H

#include <QQuickItem>
#include <boost/any.hpp>
#include <functional>
#include <vector>

class InputHandlerDelegate;

class SuggestionListController
{
public:
    SuggestionListController(QQuickItem* suggestionListView, QObject* suggestionModel, InputHandlerDelegate* inputDelegate);

    void addItem(const QString& text, std::function<void(boost::any)> handler, boost::any data);
    void clearList();
    
private:
    struct SuggestingItem
    {
        std::function<void(boost::any)> handler;
        boost::any data;
    };
    void onSuggestionItemClicked(int index);

    QQuickItem* suggestionListView_;
    QObject* suggestionModel_;
    InputHandlerDelegate* inputDelegate_;
    std::vector<SuggestingItem> suggestingItems_;
};

#endif // SUGGESTIONLISTCONTROLLER_H