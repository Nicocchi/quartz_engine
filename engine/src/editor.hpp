#pragma once

#include <iostream>
#include <unordered_map>

// struct AssetNode
// {
//     std::filesystem::path path;
//     bool isDirectory;
//     std::vector<std::unique_ptr<AssetNode>> children;
//     AssetNode *parent = nullptr;
// };

struct ANode
{
    std::filesystem::path path;
    std::string filename;
    bool isDirectory;
    // ANode *parent = nullptr;
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

    // std::unordered_map<std::string, AssetNode*> assetMap;
    // std::unique_ptr<AssetNode> rootNode;
    // ANode current_directory_node;
    std::vector<ANode> files;

    // std::unordered_map<std::string, std::filesystem::path> asset_index;

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
} editor;

void init_editor(Window *window);
void show_editor(render_context *context, int fps, game_memory *GM);
void close_editor();
