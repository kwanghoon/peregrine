// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#include "echo.h"
#include <QDebug>

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable, const PG_PLUGIN_CONFIGS* pluginConfigs)
{
    qDebug() << "Initialize echo plugin";
    return 0;
}

int RunAction(const char* actionId, const PG_ACTION_ARGUMENT_SET* argumentSet)
{
    const char* data = argumentSet->arguments[0].value;
    qDebug() << "action id: " << actionId << ", data: " << data;
    return 0;
}
