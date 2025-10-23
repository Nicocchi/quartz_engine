#include "window.hpp"

Window window;
bool create_window(const char *title, int width, int height)
{
    
    window.title = title;
    window.width = width;
    window.height = height;
    window.closed = false;

    if (!glfwInit())
    {
        return false;
    }

    window.window = glfwCreateWindow(window.width, window.height, window.title, nullptr, nullptr);
    if (!window.window)
    {
        return false;
    }

    glfwMakeContextCurrent(window.window);
    glfwSwapInterval(0);

    return true;
}

bool window_closed()
{
    return glfwWindowShouldClose(window.window) == 1;
}

void window_update()
{
    glfwPollEvents();
    glfwSwapBuffers(window.window);
}