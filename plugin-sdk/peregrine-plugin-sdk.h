#pragma once

typedef int(*SetHeaderTextFunc)(const char* text);

struct PG_FUNC_TABLE
{
    SetHeaderTextFunc fpSetHeaderText;
};

struct PG_SUGGESTION_ITEM
{
    char* displayText;
    size_t token;
};

enum PG_BEHAVIOR_ON_RETURN
{
    PG_REMAIN = 0,
    PG_DISAPPEAR = 1,
};