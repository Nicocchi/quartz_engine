#pragma once

// DLL
void* platform_load_dynamic_library(char* dll);
void* platform_load_dynamic_function(void* dll, char* funName);
bool platform_free_dynamic_library(void* dll);