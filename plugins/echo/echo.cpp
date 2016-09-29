#include "echo.h"
#include <QDebug>

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable, const PG_PLUGIN_CONFIGS* pluginConfigs)
{
    qDebug() << "Initialize echo plugin";
    return 0;
}

int RunAction(const char* actionId, const char* data)
{
    qDebug() << "action id: " << actionId << ", data: " << data;
    return 0;
}
