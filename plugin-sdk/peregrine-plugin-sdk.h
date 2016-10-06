#pragma once
#include <string.h>

typedef int(*SetHeaderTextFunc)(const char* text);
typedef int(*InvokeQmlFunc)(const char* funcName, const char* jsonArg);

struct PG_FUNC_TABLE
{
    SetHeaderTextFunc fpSetHeaderText;
    InvokeQmlFunc fpInvokeQmlFunc;
};

struct PG_PLUGIN_CONFIGS
{
    int maxSuggestions;
};

struct PG_SUGGESTION_ITEM
{
    char* displayText;
    char* imagePath;
    size_t token;
    char* completeText;
};

enum PG_BEHAVIOR_ON_RETURN
{
    PG_REMAIN = 0,
    PG_DISAPPEAR = 1,
};

struct PG_ARGUMENT
{
    const char* name;
    const char* value; // encoded in utf8
};

struct PG_ACTION_ARGUMENT_SET
{
    int n;
    PG_ARGUMENT* arguments;
};

inline const char* PgGetArgumentValue(const PG_ACTION_ARGUMENT_SET* args, const char* name)
{
    for (int i = 0; i < args->n; i++)
    {
        if (strcmp(name, args->arguments[i].name) == 0)
        { 
            return args->arguments[i].value;
        }
    }
    return (const char*)0;
}