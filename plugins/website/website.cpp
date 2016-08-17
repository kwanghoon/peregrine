#include "website.h"
#include <QDebug>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#	pragma comment(lib, "shell32.lib")
#endif

namespace
{
    const struct PG_FUNC_TABLE* funcTable;
}

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable)
{
    qDebug() << "Initialize website plugin";
    ::funcTable = funcTable;
    return 0;
}

int RunAction(const char* actionId, const char* data)
{
    qDebug() << "action id: " << actionId << ", data: " << data;
    if (strcmp(actionId, "website") == 0)
    {
        char headerText[120];
        sprintf(headerText, "visit '%s'", data);
        ::funcTable->fpSetHeaderText(headerText);

#       ifdef Q_OS_WIN
        int ret = (int)::ShellExecuteA(0, NULL, data, NULL, NULL, SW_NORMAL);
#       else
        qDebug() << "Not implemented";
#       endif

        return 0;
    }
    return -1;
}
