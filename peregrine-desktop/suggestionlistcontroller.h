#ifndef SUGGESTIONLISTCONTROLLER_H
#define SUGGESTIONLISTCONTROLLER_H

#include <QQuickItem>

class SuggestionListController
{
public:
    SuggestionListController(QQuickItem* suggestionListView, QObject* suggestionModel);

    void add(const QString& text);
    void clear();
    
private:
    QQuickItem* suggestionListView_;
    QObject* suggestionModel_;
};

#endif // SUGGESTIONLISTCONTROLLER_H