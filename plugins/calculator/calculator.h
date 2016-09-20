#pragma once

#include "calculator_global.h"
#include <peregrine-plugin-sdk.h>

#ifdef __cplusplus
extern "C"
{
#endif

CALCULATORSHARED_EXPORT
int InitializePlugin(const struct PG_FUNC_TABLE* funcTable);

CALCULATORSHARED_EXPORT
int RunAction(const char* actionId, const char* data);

#ifdef __cplusplus
}
#endif
