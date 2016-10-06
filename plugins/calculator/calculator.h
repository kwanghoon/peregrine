#pragma once

#include "calculator_global.h"
#include <peregrine-plugin-sdk.h>

#ifdef __cplusplus
extern "C"
{
#endif

CALCULATORSHARED_EXPORT
int InitializePlugin(const struct PG_FUNC_TABLE* funcTable, const PG_PLUGIN_CONFIGS* pluginConfigs);

CALCULATORSHARED_EXPORT
int RunAction(const char* actionId, const PG_ACTION_ARGUMENT_SET* argumentSet);

#ifdef __cplusplus
}
#endif
