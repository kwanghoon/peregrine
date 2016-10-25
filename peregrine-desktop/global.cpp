// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

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
        return "2016.10.25.02";
    }
}

#ifdef Q_OS_WIN
#   pragma comment(lib, "user32.lib")
#endif // Q_OS_WIN
