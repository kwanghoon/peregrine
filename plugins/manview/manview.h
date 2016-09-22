#pragma once

#include "manview_global.h"
#include <peregrine-plugin-sdk.h>

#ifdef __cplusplus
extern "C"
{
#endif

MANVIEWSHARED_EXPORT
int InitializePlugin(const struct PG_FUNC_TABLE* funcTable);

MANVIEWSHARED_EXPORT
int RunAction(const char* actionId, const char* data);

#ifdef __cplusplus
}
#endif
