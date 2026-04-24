#pragma once

#include "../vendor/glad/glad.h"
#define GL_SILENCE_DEPRECATION
#include "../vendor/glfw/glfw3.h"

#include "platform.hpp"
#include "raymath_ext.h"
#include "logger.hpp"

struct Window
{
    const char *title;
    int width, height;
    int vwidth, vheight;
    GLFWwindow *window;
    bool closed;
    input_state *Input;
};

bool create_window(Window *window, const char *title, int width, int height, EngineLogger *engineLogger);
bool window_closed(Window *window);
void window_poll();
void window_clear();
void window_swap_buffers(Window *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void window_clear(Vector3 color);
