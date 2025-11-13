#pragma once

struct Window
{
    const char *title;
    int width, height;
    int vwidth, vheight;
    GLFWwindow *window;
    bool closed;
    // FrameBuffer *fbo;
};

bool create_window(Window *window, const char *title, int width, int height);
bool window_closed(Window *window);
void window_poll();
void window_clear();
void window_swap_buffers(Window *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);