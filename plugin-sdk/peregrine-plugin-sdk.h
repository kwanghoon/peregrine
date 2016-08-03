#pragma once

typedef int (__stdcall *RegisterActionFunc)(const char* id, const char* name);

struct PG_FUNC_TABLE
{
    RegisterActionFunc fpRegisterAction;
};