#pragma once

typedef int (__stdcall *RegisterActionFunc)(const char* id);

struct PG_FUNC_TABLE
{
    RegisterActionFunc fpRegisterAction;
};