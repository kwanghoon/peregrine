#pragma once

#include "file_global.h"
#include <peregrine-plugin-sdk.h>

#ifdef __cplusplus
extern "C"
{
#endif

fileSHARED_EXPORT
int InitializePlugin(const struct PG_FUNC_TABLE* funcTable);

fileSHARED_EXPORT
int RunAction(const char* actionId, const char* data);

#ifdef __cplusplus
}
#endif