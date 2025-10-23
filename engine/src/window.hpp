#pragma once

#include "../vendor/glfw/glfw3.h"

struct Window
{
    const char *title;
    int width, height;
    GLFWwindow *window;
    bool closed;
};

bool create_window(const char *title, int width, int height);
bool window_closed();
void window_update();