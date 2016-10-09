// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#pragma once

#include "userconfig.h"

class SuggestionListController;
class ConfigManager;
class SyncManager;

namespace global
{
    extern UserConfig userConfig;
    extern SuggestionListController* suggestionListController;
    ConfigManager& GetConfigManager();
    SyncManager& GetSyncManager();
    const char* getAppVersion();

}
