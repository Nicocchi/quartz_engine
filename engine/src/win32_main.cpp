#include "platform.hpp"

#include "windows.h"
#include <stdio.h>

#include "pch.hpp"
#include "unity_build.hpp"


void* loadDLL(char* dll)
{
    HMODULE result = LoadLibraryA(dll);
    // QZ_ASSERT(result, "Failed to load dll: %s", dll);

    return result;
}

void* loadDLLFunc(void* dll, char* funName)
{
    FARPROC proc = GetProcAddress((HMODULE)dll, funName);
    // QZ_ASSERT(proc, "Failed to load function: %s from DLL", funName);

    return (void*)proc;
}

bool freeDLL(void* dll)
{
    BOOL freeResult = FreeLibrary((HMODULE)dll);
    // QZ_ASSERT(freeResult, "Failed to FreeLibrary");

    return (bool)freeResult;
}

int main()
{
    Window *window = new Window();
    if (!create_window(window, "Quartz", 1280, 720))
    {
        // error
        return -1;
    }
    init_editor(window);

    game_memory GameMemory;
    GameMemory.storageSize = 2048LL * 1024LL * 1024LL;
    GameMemory.storage = VirtualAlloc(0, GameMemory.storageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    GameMemory.renderSize = 2048LL * 1024LL * 1024LL;
    GameMemory.renderStorage = VirtualAlloc(0, GameMemory.renderSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    game_update_and_render *gameUpdate = nullptr;

    render_context *render_state = (render_context*)GameMemory.renderStorage;


    void *gameDLL = nullptr;
    gameDLL = loadDLL("game.dll");
    gameUpdate = (game_update_and_render *)loadDLLFunc(gameDLL, "GameUpdateAndRender");

    float deltaTime = 0.0f;
    float previousTime = glfwGetTime();
    int frameCount = 0;
    int fps = 0;
    float currentFrame = 0.0f;
    float lastFrame = 0.0f;

    init_renderer(render_state, window->vwidth, window->vheight);

    render_camera camera;
    camera.position.x = 0.0f;
    camera.position.y = 0.0f;
    // camera.position = glm::vec2(0.0f, 0.0f);
    camera.zoom = 1.0f;
    camera.rotation = 0.0f;
    render_state->camera = &camera;

    // Matrix projection = MatrixOrtho(0.0f, 1280, 720,
    //                                 0.0f, -1.0f, 1.0f);

    // glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(window->vwidth), static_cast<float>(window->vheight),
    //                                     0.0f, -1.0f, 1.0f);

    // render_state->projection = projection;

    while(!window_closed(window))
    {
        currentFrame = glfwGetTime();
        frameCount++;
        if (currentFrame - previousTime >= 1.0)
        {
            // display frame count
            fps = frameCount;
            // printf("%d fps\n", frameCount);
            frameCount = 0;
            previousTime = currentFrame;
        }
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // poll -> update -> render
        window_poll();
        render_state->projection = create_projection(camera.zoom, camera.position, camera.rotation, 1280, 720);
        // render_state->width = window->vwidth;
        // render_state->height = window->vheight;

        render_state->vertexCount = 0;
        render_state->indexCount = 0;

        gameUpdate(&GameMemory, deltaTime);

        // render logic
        bind_framebuffer(render_state->fbo, window->vwidth, window->vheight);
        window_clear(render_state->color);
        draw_scene(render_state);
        unbind_framebuffer();
        show_editor(render_state, fps, &GameMemory);

        window_swap_buffers(window);
    }

    close_editor();

    // delete editor;
    delete window;

    freeDLL(gameDLL);
}
