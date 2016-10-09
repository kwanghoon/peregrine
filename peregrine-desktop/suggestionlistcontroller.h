// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#ifndef SUGGESTIONLISTCONTROLLER_H
#define SUGGESTIONLISTCONTROLLER_H

#include <QObject>
#include <boost/any.hpp>
#include <functional>
#include <vector>

class InputHandlerDelegate;
class QQuickWidget;
class QQuickItem;

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
    bool isVisible() const;
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