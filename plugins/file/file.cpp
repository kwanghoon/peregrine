#include "file.h"
#include <QDir>
#include <QDebug>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <string>
#include <codecvt>

#ifdef Q_OS_WIN
#   include <windows.h>
#   include <shellapi.h>
#	pragma comment(lib, "shell32.lib")
#endif

using namespace std;
using namespace boost;

namespace
{
    size_t nextToken = 1000;

    std::map<size_t/* token */, std::string/* path */> suggested;

    string convertWstrToUtf8(const wstring& ucs2)
    {
        wstring_convert<codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(ucs2);
    }
    wstring convertUtf8ToWstr(const char* s)
    {
        wstring_convert<codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(s);
    }
}

int InitializePlugin(const struct PG_FUNC_TABLE* funcTable)
{
    qDebug() << "Initialize file plugin";
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

char* GetFileIconPath(filesystem::path p)
{
    const char* kKnownExtensions[] = {
        "exe", "html", "iso", "pdf", "xml", "txt", "zip", "mp3", "mp4",
        "ppt", "doc", "ai", "avi", "css", "csv", "pdf", "png", 
        "rtf", "xls", "svg",
    };
    if (filesystem::is_directory(p))
    {
        return strdup("plugins/file/filetype-icons/files.svg");
    }

    std::string ext = p.extension().string();
    if (!ext.empty())
    {
        ext = ext.substr(1);
        auto it = find_if(std::begin(kKnownExtensions), std::end(kKnownExtensions), [&](const char* filetype) {
            return ext == filetype;
        });
        if (it != std::end(kKnownExtensions))
        {
            // #TODO: Buffer Overrun
            char s[300];
            sprintf(s, "plugins/file/filetype-icons/%s.svg", *it);
            return strdup(s);
        }
    }

    return strdup("plugins/file/filetype-icons/file.svg");
}

int GetSuggestionItems(const char* currentActionId, const char* input, int* n, struct PG_SUGGESTION_ITEM** items)
{
    *n = 0;
    *items = nullptr;
    suggested.clear();

    if (strcmp(currentActionId, "file") != 0)
    {
        return -1;
    }

    // 1. input이 valid한 패쓰인지 확인
    string s = input;
    while (!s.empty() && s.back() == ' ')
    { // remove trailig spaces
        s.pop_back();
    }
    if (!isValidPath(s))
    {
        return 0;
    }

    // 3. valid 하면 parent 디렉토리를 구함 = p
    filesystem::path path = convertUtf8ToWstr(input);
    filesystem::path parentDir;
    if (path.root_path() == path)
    {
        parentDir = path;
    }
    else
    {
        parentDir = path.parent_path();
    }
    if (!filesystem::exists(parentDir))
    {
        return 0;
    }
    
    // p 아래 'filename*' 조건으로 모든 파일을 검색
    string filenameFrontPart = convertWstrToUtf8(path.filename().wstring());
    if (filenameFrontPart == "." || filenameFrontPart == "/")
    {
        filenameFrontPart.clear();
    }
    const int kMaxItems = 10;
    *items = (PG_SUGGESTION_ITEM*)malloc(sizeof(PG_SUGGESTION_ITEM) * kMaxItems);
    for (auto it = filesystem::directory_iterator(parentDir); it != filesystem::directory_iterator(); it++)
    {
        string childFilename = convertWstrToUtf8(it->path().filename().wstring());
        if (istarts_with(childFilename, filenameFrontPart))
        {
            string s = str(boost::format("<b>%s</b>%s") 
                % childFilename.substr(0, filenameFrontPart.length()) 
                % childFilename.substr(filenameFrontPart.length()));
            (*items)[*n].displayText = strdup(s.c_str());
            (*items)[*n].imagePath = GetFileIconPath(it->path());
            size_t token = nextToken++;
            suggested[token] = boost::filesystem::system_complete(it->path()).string();
            (*items)[*n].token = token;
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
    assert(suggested.count(token) != 0);
    string path = suggested[token];
#   ifdef Q_OS_WIN
    int ret = (int)::ShellExecuteA(0, NULL, path.c_str(), NULL, NULL, SW_NORMAL);
#   endif
    return PG_BEHAVIOR_ON_RETURN::PG_DISAPPEAR;
}
