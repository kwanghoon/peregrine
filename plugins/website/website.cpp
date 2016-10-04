#include "website.h"
#include <QDebug>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#   pragma comment(lib, "shell32.lib")
#else
#   include <stdlib.h>
#endif

#include <algorithm>

using namespace std;

namespace
{
    const struct PG_FUNC_TABLE* funcTable;

    string url_encode(const unsigned char* s) 
    {
        string ret;
        ret.reserve(80);
        while (*s)
        {
            unsigned char c = *s;
            const char reserved[] = "!*'();:@&=+$,/?#[]" "-_.~";

            if (isalnum(c) || std::find(std::begin(reserved), std::end(reserved), (char)c) != std::end(reserved))
            {
                ret += c;
            }
            else
            {
                auto toHex = [](int x) { return x < 10 ? '0' + x : 'A' + (x - 10); };
                ret += '%';
                ret += toHex(c >> 4);
                ret += toHex(c & 0x0F);
            }
            s++;
        }
        return ret;
    }
}

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable, const PG_PLUGIN_CONFIGS*)
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

        const char* p = strstr(data, "://");
        string url(data, p + 3);
        string urlEncoded = url_encode(reinterpret_cast<const unsigned char*>(p + 3));
        url += urlEncoded;
#       ifdef Q_OS_WIN
        int ret = (int)::ShellExecuteA(0, NULL, url.c_str(), NULL, NULL, SW_NORMAL);
#       else
        system(QString("firefox \"%1\" &").arg(url.c_str()).toStdString().c_str());
#       endif

        return 0;
    }
    return -1;
}
