#ifndef SUGGESTIONLISTCONTROLLER_H
#define SUGGESTIONLISTCONTROLLER_H

#include <QQuickItem>
#include <boost/any.hpp>
#include <functional>
#include <vector>

class InputHandlerDelegate;
class QQuickWidget;

class SuggestionListController : public QObject
{
    Q_OBJECT

public:
    SuggestionListController(QQuickWidget* suggestionBox, QQuickItem* suggestionListView, 
        QObject* suggestionModel, InputHandlerDelegate* inputDelegate);

    enum class SuggestionRunType
    {
        Enter,
        Tab
    };
    void addItem(const QString& text, const QString& imagePath,
        std::function<int(SuggestionRunType, boost::any)> handler, boost::any data);
    void clearList();
    void selectUp();
    void selectDown();
    int getCurrentIndex() const;
    int runSelected(SuggestionListController::SuggestionRunType type);
    void setVisible(bool visible);
    int getCount() const { return suggestingItems_.size(); }
    
private:
    struct SuggestingItem
    {
        std::function<int(SuggestionRunType, boost::any)> handler;
        boost::any data;
    };
    int onSuggestionItemClicked(int type, int index);

    QQuickWidget* suggestionBox_;
    QQuickItem* suggestionListView_;
    QObject* suggestionModel_;
    InputHandlerDelegate* inputDelegate_;
    std::vector<SuggestingItem> suggestingItems_;
};

#endif // SUGGESTIONLISTCONTROLLER_H