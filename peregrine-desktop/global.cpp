#include "global.h"
#include "configmanager.h"
#include "syncmanager.h"
#include <QGlobalStatic>

Q_GLOBAL_STATIC(ConfigManager, configManagerInstance);
Q_GLOBAL_STATIC(SyncManager, syncManagerInstance)

namespace global
{
    UserConfig userConfig;
    SuggestionListController* suggestionListController = nullptr;

    ConfigManager& GetConfigManager()
    {
        return *configManagerInstance;
    }

    SyncManager& GetSyncManager()
    {
        return *syncManagerInstance;
    }

    const char* getAppVersion()
    {
        return "2016.10.08.01";
    }
}

#ifdef Q_OS_WIN
#   pragma comment(lib, "user32.lib")
#endif // Q_OS_WIN
