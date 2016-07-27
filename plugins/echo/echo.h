#pragma once

#include "echo_global.h"
#include <peregrine-plugin-sdk.h>

#ifdef __cplusplus
extern "C"
{
#endif

ECHOSHARED_EXPORT
int InitializePlugin(const struct PG_FUNC_TABLE* funcTable);

ECHOSHARED_EXPORT
int RunAction(const char* actionId, const char* data);

#ifdef __cplusplus
}
#endif
