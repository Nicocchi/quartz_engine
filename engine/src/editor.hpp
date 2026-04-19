#pragma once

struct ANode
{
    std::filesystem::path path;
    std::string filename;
    bool isDirectory;
    Texture2D *texture = nullptr;
};

struct ContentBrowser2
{
    std::filesystem::path asset_path;
    std::filesystem::path current_directory;
    std::filesystem::path current_filename;
    int padding;
    int thumbnail_size;
    Texture2D *folder_icon;
    Texture2D *file_icon;
    std::vector<ANode> files;

} content_browser;

struct Editor
{
    ImGuiIO io;
    ImGuiStyle style;
    ImFont *iconFont;

    const char *glsl_version = "#version 460";
    ImVec4 color;
    Window *window;
    bool firstTime;

    // Temp
    Texture2D *sprite_temp;
    Texture2D *tilemap_temp;
    Tile tile;
    int selected_tile = -1;
    bool paint_mode = false;
    bool draw_grid = true;

    ImGuiTextBuffer Buf;
    std::vector<std::string> log_messages;
    bool ScrollToBottom;

    bool save_modal_popup = false;
    bool load_modal_popup = false;
    std::string save_name_buffer = "";
} editor;

void init_editor(Window *window);
void show_editor(render_context *context, int fps, game_memory *GM);
void close_editor();
