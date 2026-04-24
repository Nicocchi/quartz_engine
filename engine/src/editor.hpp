#pragma once

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../vendor/imgui/imgui_internal.h"
#include "../vendor/imgui/imgui_stdlib.h"

#include "texture_2d.hpp"
#include "renderer_2d.hpp"
#include "entity.hpp"
#include "window.hpp"
#include "logger.hpp"

#include <filesystem>

struct ANode
{
    std::filesystem::path path;
    std::string filename;
    Texture2D *texture = nullptr;
    bool isDirectory;
};

struct ContentBrowser
{
    std::filesystem::path asset_path;
    std::filesystem::path current_directory;
    std::filesystem::path current_filename;

    std::vector<ANode> files;
    
    Texture2D *folder_icon;
    Texture2D *file_icon;
    
    int padding;
    int preview_size;
    int thumbnail_size;
};

static ContentBrowser content_browser;

struct Editor
{
    Window *window;

    // Imgui
    ImGuiIO io;
    ImGuiStyle style;
    ImFont *iconFont;

    const char *glsl_version = "#version 460";
    ImVec4 color;
    bool firstTime; // First time editor loads

    // Entity
    Texture2D *sprite_temp;

    // Tilemap
    Texture2D *tilemap_temp;
    Tile tile;
    int selected_tile = -1;
    bool paint_mode = false;
    bool draw_grid = true;
    
    // Modals
    bool save_modal_popup = false;
    bool load_modal_popup = false;
    std::string save_name_buffer = "";
    
    // Console
    bool ScrollToBottom;
    bool auto_scroll;
    int prev_displayed_count;
    int displayed_count;
    bool user_at_bottom;
    std::vector<std::string> log_messages;
};

static Editor editor;

void init_editor(Window *window);
void show_editor(render_context *context, int fps, input_state *Input, game_memory *GM, Window *window, EngineLogger *engineLogger);
void close_editor();
void process_editor_input(render_context *render_state, input_state *Input, float dt);
