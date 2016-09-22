#include "manview.h"
#include <QDebug>
#include <string>

const struct PG_FUNC_TABLE* g_funcTable;

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable)
{
    g_funcTable = funcTable;
    return 0;
}

int RunAction(const char* actionId, const char* data)
{
    return PG_BEHAVIOR_ON_RETURN::PG_REMAIN;
}
