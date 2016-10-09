// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at 
// https://mozilla.org/MPL/2.0/.

#include "website.h"
#include <QDebug>
#include <boost/locale.hpp>

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

int RunAction(const char* actionId, const PG_ACTION_ARGUMENT_SET* argumentSet)
{
    const char* urlInUTF8 = PgGetArgumentValue(argumentSet, "input_text");
    assert(urlInUTF8);
    qDebug() << "action id: " << actionId << ", data: " << urlInUTF8;
    if (strcmp(actionId, "website") == 0)
    {
        char headerText[120];
        sprintf(headerText, "visit '%s'", urlInUTF8);
        ::funcTable->fpSetHeaderText(headerText);

        const char* encoding = PgGetArgumentValue(argumentSet, "encoding");
        string url;
        if (encoding && strcmp(encoding, "EUC-KR") == 0)
        {
            url = boost::locale::conv::from_utf(urlInUTF8, "EUC-KR");
        }
        else
        {
            url = urlInUTF8;
        }

        const char* p = strstr(url.c_str(), "://");
        string percentEncoded = string(url.c_str(), p + 3) +
            url_encode(reinterpret_cast<const unsigned char*>(p + 3));
#       ifdef Q_OS_WIN
        int ret = (int)::ShellExecuteA(0, NULL, percentEncoded.c_str(), NULL, NULL, SW_NORMAL);
#       else
        system(QString("firefox \"%1\" &").arg(percentEncoded.c_str()).toStdString().c_str());
#       endif

        return 0;
    }
    return -1;
}
