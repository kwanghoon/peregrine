#pragma once

typedef int (__stdcall *RegisterActionFunc)(const char* id, const char* name);

struct PG_FUNC_TABLE
{
    RegisterActionFunc fpRegisterAction;
};

struct PG_SUGGESTION_ITEM
{
    char* displayText;
    size_t token;
};