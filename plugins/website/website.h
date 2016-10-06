#pragma once

#include "website_global.h"
#include <peregrine-plugin-sdk.h>

#ifdef __cplusplus
extern "C"
{
#endif

WEBSITESHARED_EXPORT
int InitializePlugin(const struct PG_FUNC_TABLE* funcTable, const PG_PLUGIN_CONFIGS* pluginConfigs);

WEBSITESHARED_EXPORT
int RunAction(const char* actionId, const PG_ACTION_ARGUMENT_SET* argumentSet);

#ifdef __cplusplus
}
#endif