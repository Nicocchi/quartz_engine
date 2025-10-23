#define IMPORTING_DLL
#include "defines.hpp"

#include "platform.hpp"
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include <cstdio>

#include "window.cpp"

#include "../../sandbox/src/game.h"

typedef decltype(update) update_type;
static update_type *update_ptr;

void update()
{
    update_ptr();
}

int main()
{
    static void *gameDLL;
    gameDLL = platform_load_dynamic_library("game.dll");
    update_ptr = (update_type *)platform_load_dynamic_function(gameDLL, "update");

    create_window("Quartz Engine", 960, 540);

    while (!window_closed())
    {
        window_update();
    }

    update();
    return 0;
}