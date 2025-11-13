#pragma once

struct Editor
{
    ImGuiIO io;
    ImGuiStyle style;
    ImFont *iconFont;

    const char *glsl_version = "#version 460";
    ImVec4 color;
    Window *window;
    bool firstTime;
} editor;

void init_editor(Window *window);
void show_editor(render_context *context, int fps, game_memory *GM);
void close_editor();