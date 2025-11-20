#include "platform.hpp"

#include "windows.h"
#include <stdio.h>

#include "pch.hpp"
#include "unity_build.hpp"


struct game_code
{
    HMODULE dll;
    FILETIME lastWriteTime;
    game_update_and_render *update;
};

/**
 * Returns the last write time of a file.
 */
FILETIME getLastWriteTime(const char *filename)
{
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesExA(filename, GetFileExInfoStandard, &data))
    {
        return data.ftLastWriteTime;
    }
    FILETIME empty = {};
    return empty;
}

/**
 * Loads the game dll and sets up the game_code struct.
 */
game_code loadGameCode(const char *dllName, const char *tempDllName)
{
    game_code gc = {};

    CopyFileA(dllName, tempDllName, FALSE);

    gc.dll = LoadLibraryA(tempDllName);
    if (gc.dll)
    {
        gc.update = (game_update_and_render*) GetProcAddress(gc.dll, "GameUpdateAndRender");
        gc.lastWriteTime = getLastWriteTime(dllName);
    }
    else
    {
        gc.update = nullptr;
    }

    return gc;
}

void unloadGameCode(game_code *gc)
{
    if (gc->dll)
    {
        FreeLibrary(gc->dll);
        gc->dll = nullptr;
        gc->update = nullptr;
    }
}

int main()
{

    // Window Initialization
    Window *window = new Window();
    if (!create_window(window, "Quartz", 1280, 720))
    {
        // error
        return -1;
    }

    // GameMemory/Storage Initialization
    game_memory GameMemory;
    GameMemory.storageSize = 2048LL * 1024LL * 1024LL;
    GameMemory.storage = VirtualAlloc(0, GameMemory.storageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    GameMemory.renderSize = 2048LL * 1024LL * 1024LL;
    GameMemory.renderStorage = VirtualAlloc(0, GameMemory.renderSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    render_context *render_state = (render_context*)GameMemory.renderStorage;

    game_code gameCode = loadGameCode("game.dll", "game_temp.dll");

    // Delta Time/FPS
    float deltaTime = 0.0f;
    float previousTime = glfwGetTime();
    int frameCount = 0;
    int fps = 0;
    float currentFrame = 0.0f;
    float lastFrame = 0.0f;

    // Renderer Initialization
    init_renderer(render_state, window->vwidth, window->vheight);

    render_camera camera;
    camera.position = { 0.0f, 0.0f };
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;
    render_state->camera = &camera;

    // IMGUI Editor
    // TODO (Nico): Add option to enable/disable
    init_editor(window);

    while(!window_closed(window))
    {
        // DeltaTime/FPS Calc
        currentFrame = glfwGetTime();
        frameCount++;
        if (currentFrame - previousTime >= 1.0)
        {
            // display frame count
            fps = frameCount;
            frameCount = 0;
            previousTime = currentFrame;
        }
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Hot reloading
        FILETIME newTime = getLastWriteTime("game.dll");
        if (CompareFileTime(&newTime, &gameCode.lastWriteTime) != 0)
        {
            unloadGameCode(&gameCode);
            gameCode = loadGameCode("game.dll", "game_temp.dll");
            GameMemory.isInit = false;
            printf("Hot reloaded game.dll\n");
        }

        // Things should go in this order:
        //  Poll -> Update -> Render
        window_poll();
        render_state->projection = create_projection(camera.zoom, camera.position, camera.rotation, 1280, 720);

        render_state->vertexCount = 0;
        render_state->indexCount = 0;

        // Call game code methods
        if (gameCode.update)
        {
            gameCode.update(&GameMemory, window->Input, deltaTime);
        }

        // Render logic
        bind_framebuffer(render_state->fbo, window->vwidth, window->vheight);
        window_clear(render_state->color);

        draw_scene(render_state);

        unbind_framebuffer();

        // IMGUI Editor
        // TODO (Nico): Add option to enable/disable
        show_editor(render_state, fps, window->Input, &GameMemory);

        window_swap_buffers(window);
    }

    // IMGUI Editor
    // TODO (Nico): Add option to enable/disable
    close_editor();

    // delete editor;
    delete window;

    unloadGameCode(&gameCode);
}
