#include <QString>
#include <QtTest>

class PeregrineTest : public QObject
{
    Q_OBJECT

public:
    PeregrineTest();

private Q_SLOTS:
    void suggestionKeywordMatchTest();
    void expressionTest();
};
