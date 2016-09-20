#include "calculator.h"
#include <QDebug>

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable)
{
    qDebug() << "Initialize calculator plugin";
    return 0;
}

int RunAction(const char* actionId, const char* data)
{
    qDebug() << "action id: " << actionId << ", data: " << data;
    return 0;
}
