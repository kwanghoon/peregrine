#pragma once

#include "userconfig.h"

class SuggestionListController;
class ConfigManager;

namespace global
{
    extern UserConfig userConfig;
    extern SuggestionListController* suggestionListController;
    ConfigManager& GetConfigManager();
}
