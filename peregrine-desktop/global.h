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
}
