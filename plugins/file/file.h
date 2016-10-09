// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#pragma once

#include "file_global.h"
#include <peregrine-plugin-sdk.h>

#ifdef __cplusplus
extern "C"
{
#endif

FILESHARED_EXPORT
int InitializePlugin(const struct PG_FUNC_TABLE* funcTable, const PG_PLUGIN_CONFIGS* pluginConfigs);

FILESHARED_EXPORT
int RunAction(const char* actionId, const PG_ACTION_ARGUMENT_SET* argumentSet);

FILESHARED_EXPORT
int GetSuggestionItems(const char* currentActionId, const char* input, int* n, struct PG_SUGGESTION_ITEM** items);

FILESHARED_EXPORT
int RunSuggestion(size_t token);

#ifdef __cplusplus
}
#endif