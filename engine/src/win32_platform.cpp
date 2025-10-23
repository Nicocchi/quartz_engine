#include "platform.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

void* platform_load_dynamic_library(char* dll)
{
    HMODULE result = LoadLibraryA(dll);

    return result;
}

void* platform_load_dynamic_function(void* dll, char* funName)
{
    FARPROC proc = GetProcAddress((HMODULE)dll, funName);

    return (void*)proc;
}

bool platform_free_dynamic_library(void* dll)
{
    BOOL freeResult = FreeLibrary((HMODULE)dll);

    return (bool)freeResult;
}