#include "global.h"
#include "configmanager.h"
#include <QGlobalStatic>

Q_GLOBAL_STATIC(ConfigManager, configManagerInstance);

namespace global
{
    UserConfig userConfig;
    SuggestionListController* suggestionListController = nullptr;

    ConfigManager& GetConfigManager()
    {
        return *configManagerInstance;
    }
}
