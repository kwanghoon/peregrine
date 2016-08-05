#include "file.h"
#include <QDir>
#include <QDebug>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <string>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#	pragma comment(lib, "shell32.lib")
#endif

using namespace std;
using namespace boost;

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable)
{
    qDebug() << "Initialize file plugin";
    funcTable->fpRegisterAction("file", "file");
    return 0;
}

int RunAction(const char* actionId, const char* data)
{
    qDebug() << "action id: " << actionId << ", data: " << data;
    if (strcmp(actionId, "file") == 0)
    {
#       ifdef Q_OS_WIN
        int ret = (int)::ShellExecuteA(0, NULL, data, NULL, NULL, SW_NORMAL);
#       else
        qDebug() << "Not implemented";
#       endif

        return 0;
    }
    return -1;
}

bool isValidPath(const std::string& _path)
{
    boost::filesystem::path path(_path);

    if (!path.is_absolute())
    {
        return false;
    }

    while (path != path.root_path())
    {
        std::string name = path.filename().generic_string();
        if (!boost::filesystem::native(name))
        {
            return false;
        }
        path = path.parent_path();
    }
    return true;
}

int GetSuggestionItems(const char* currentActionId, const char* input, int* n, struct PG_SUGGESTION_ITEM** items)
{
    *n = 0;
    *items = nullptr;

    if (strcmp(currentActionId, "file") != 0)
    {
        return -1;
    }

    // 1. input�� valid�� �о����� Ȯ��
    if (!isValidPath(input))
    {
        return 0;
    }

    // 3. valid �ϸ� parent ���丮�� ���� = p
    filesystem::path path = input;
    filesystem::path parentDir = path.parent_path();
    if (!filesystem::exists(parentDir))
    {
        return 0;
    }
    
    // p �Ʒ� 'filename*' �������� ��� ������ �˻�
    string filenameFrontPart = path.filename().string();
    const int kMaxItems = 10;
    *items = (PG_SUGGESTION_ITEM*)malloc(sizeof(PG_SUGGESTION_ITEM) * kMaxItems);
    for (auto it = filesystem::directory_iterator(parentDir); it != filesystem::directory_iterator(); it++)
    {
        string childFilename = it->path().filename().string();
        if (istarts_with(childFilename, filenameFrontPart))
        {
            (*items)[*n].displayText = strdup(childFilename.c_str());
            (*items)[*n].token = 0; // #TODO
            (*n)++;
            if (*n >= kMaxItems)
            {
                break;
            }
        }
    }
    return 0;
}

int RunSuggestion(size_t token)
{
    // #TODO: not implemented
    return -1;
}
