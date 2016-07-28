#pragma once

#include "website_global.h"
#include <peregrine-plugin-sdk.h>

#ifdef __cplusplus
extern "C"
{
#endif

WEBSITESHARED_EXPORT
int InitializePlugin(const struct PG_FUNC_TABLE* funcTable);

WEBSITESHARED_EXPORT
int RunAction(const char* actionId, const char* data);

#ifdef __cplusplus
}
#endif