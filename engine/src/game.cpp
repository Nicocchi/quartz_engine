#include "game.hpp"
#include <random>
// unitybuild
#include <cstdio>

// Layer - z-index
// don't change original order
// secondary list of pointers to each entity and sort that based on z-index (per frame - discard after)
// generate render commands

// blending
// full blend mode - normal - additive - multiply - screen
// opacity - transparency [X]

// texture subregions - texture atlas [X]

// flip texture (horz - vert)

// gradients

// per-vertex coloring
// per-vertex upload colors (only corners) - inbetween gets interpolated

// shapes
// lines - circles - points - mesh (basically a set of triangles) - can be deformed

// Lighting
// batch rend


// input
// sound
// font
// physics - collisions - box2d
// animation
// state machines
// in-game ui
// profiling - perf/tracy (casey)
// threading

// tilemaps

void game_init(game_memory *GameMemory);

extern "C" __declspec(dllexport) GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    game_state *state = (game_state*)GameMemory->storage;
    render_context *render_state = (render_context*)GameMemory->renderStorage;


    if (!GameMemory->isInit)
    {
        game_init(GameMemory);
        GameMemory->isInit = true;
    }

    for (int i = 0; i < state->entities.size(); i++)
    {
        render_command draw_texture;
        draw_texture.type = DRAW_TEXTURE;
        draw_texture.id = state->entities[i].id;
        draw_texture.texture = state->entities[i].sprite.texture;

        Vector2 pos = state->entities[i].transform.position;
        Vector2 esize = state->entities[i].transform.size;
        float rotation = state->entities[i].transform.rotation;

        vertex *v = &render_state->vertices[render_state->vertexCount];

        // Sprite animation
        float tw = float(state->entities[i].animated_sprite.cell_width) / state->entities[i].animated_sprite.texture_width;
        float th = float(state->entities[i].animated_sprite.cell_height) / state->entities[i].animated_sprite.texture_height;
        int width = state->entities[i].animated_sprite.cell_width;
        int twidth = state->entities[i].animated_sprite.texture_width;
        int numPerRow = twidth / width;

        state->entities[i].animated_sprite.current_duration += deltaTime;



        if (state->entities[i].animated_sprite.current_duration >= state->entities[i].animated_sprite.frame_duration)
        {
            state->entities[i].animated_sprite.current_duration = 0.0f;
            state->entities[i].animated_sprite.frame_index++;

            if (state->entities[i].animated_sprite.frame_index > state->entities[i].animated_sprite.end_frame)
            {
                state->entities[i].animated_sprite.frame_index = state->entities[i].animated_sprite.start_frame;
            }
        }
        
        int frame = state->entities[i].animated_sprite.frame_index;
        
        float tx = (frame % numPerRow) * tw;
        float ty = 1.0f - ((frame / numPerRow) + 1) * th;

        tx += state->entities[i].animated_sprite.offset_x;
        ty += state->entities[i].animated_sprite.offset_y;

        v[0].position = pos;
        // Top-left
        v[0].texCoords.x = tx;
        v[0].texCoords.y = ty + th;
        v[0].color.x = state->entities[i].sprite.color.x;
        v[0].color.y = state->entities[i].sprite.color.y;
        v[0].color.z = state->entities[i].sprite.color.z;
        v[0].color.w = state->entities[i].sprite.color.w;

        v[1].position.x = pos.x + esize.x;
        v[1].position.y = pos.y;
        // Top-right
        v[1].texCoords.x = tx + tw;
        v[1].texCoords.y = ty + th;
        v[1].color.x = state->entities[i].sprite.color.x;
        v[1].color.y = state->entities[i].sprite.color.y;
        v[1].color.z = state->entities[i].sprite.color.z;
        v[1].color.w = state->entities[i].sprite.color.w;

        v[2].position.x = pos.x + esize.x;
        v[2].position.y = pos.y + esize.y;
        // Bottom-right
        v[2].texCoords.x = tx + tw;
        v[2].texCoords.y = ty;
        v[2].color.x = state->entities[i].sprite.color.x;
        v[2].color.y = state->entities[i].sprite.color.y;
        v[2].color.z = state->entities[i].sprite.color.z;
        v[2].color.w = state->entities[i].sprite.color.w;

        v[3].position.x = pos.x;
        v[3].position.y = pos.y + esize.y;
        // Bottom-left
        v[3].texCoords.x = tx;
        v[3].texCoords.y = ty;
        v[3].color.x = state->entities[i].sprite.color.x;
        v[3].color.y = state->entities[i].sprite.color.y;
        v[3].color.z = state->entities[i].sprite.color.z;
        v[3].color.w = state->entities[i].sprite.color.w;


        if (rotation != 0)
        {
            float c = cos(rotation);
            float s = sin(rotation);

            Vector2 center = pos + esize / 2;

            for (int j = 0; j < 4; j++)
            {
                Vector2 p = v[j].position;

                // Offset from center
                float offset_x = p.x - center.x;
                float offset_y = p.y - center.y;

                // Rotate
                float rx = offset_x * c - offset_y * s;
                float ry = offset_x * s + offset_y * c;

                // Reposition back around center
                v[j].position.x = center.x + rx;
                v[j].position.y = center.y + ry;
            }
        }

        unsigned int *ind = &render_state->indices[render_state->indexCount];
        ind[0] = 0 + render_state->vertexCount;
        ind[1] = 1 + render_state->vertexCount;
        ind[2] = 2 + render_state->vertexCount;
        ind[3] = 0 + render_state->vertexCount;
        ind[4] = 2 + render_state->vertexCount;
        ind[5] = 3 + render_state->vertexCount;

        render_state->indexCount += 6;

        draw_texture.offset = render_state->vertexCount;
        render_state->vertexCount += 4;

        draw_texture.count = 6;

        render_state->render_commands.push(draw_texture);
    }
}

void game_init(game_memory *GameMemory)
{
    game_state *state = (game_state*)GameMemory->storage;
    render_context *render_state = (render_context*)GameMemory->renderStorage;

    render_state->color.r = 66.0f / 255;
    render_state->color.g = 135.0f / 255;
    render_state->color.b = 245.0f / 255;

    render_shader shader;
    shader.vertexPath = "assets/shaders/basic.vs";
    shader.fragmentPath = "assets/shaders/basic.frag";
    render_state->shader = shader;

    render_command compile_shader;
    compile_shader.type = COMPILE_SHADER;
    compile_shader.shader = &render_state->shader;
    render_state->render_commands.push(compile_shader);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(-1000, 1000); // range

    // for (int i = 0; i < 20; i++)
    // {
    //     Entity entity;
    //     entity.id = state->nextID++;
    //     entity.name = "Entity 1";
    //     entity.transform.position.x = distr(gen);
    //     entity.transform.position.y = distr(gen);
    //     entity.transform.size.x = 200.0f;
    //     entity.transform.size.y = 200.0f;
    //     entity.transform.rotation = 0.0f;
    //     entity.texture = new Texture2D();
    //     entity.texture->isLoadedGPU = false;

    //     render_command load_texture;
    //     load_texture.type = UPLOAD;
    //     load_texture.filePath = "assets/textures/block.png";
    //     load_texture.texture = entity.texture;
    //     render_state->render_commands.push(load_texture);

    //     state->entities.push_back(entity);
    // }

    Entity entity;
    entity.id = state->nextID++;
    entity.name = "Player";
    entity.transform.position.x = -91;
    entity.transform.position.y = -91;
    entity.transform.size.x = 240.0f;
    entity.transform.size.y = 240.0f;

    entity.sprite.texture = new Texture2D();
    entity.sprite.texture->isLoadedGPU = false;
    entity.sprite.color = {1.0f, 1.0f, 1.0f, 1.0f};

    entity.animated_sprite.frame_index = 12;
    entity.animated_sprite.texture_width = 96;
    entity.animated_sprite.texture_height = 96;
    entity.animated_sprite.offset_x = 0.0f;
    entity.animated_sprite.offset_y = 0.0f;
    entity.animated_sprite.cell_width = 24;
    entity.animated_sprite.cell_height = 24;
    entity.animated_sprite.max_frames = 16;
    entity.animated_sprite.start_frame = 12;
    entity.animated_sprite.end_frame = 15;
    entity.animated_sprite.frame_duration = 0.25f;
    
    entity.transform.rotation = 0.0f;
    // entity.texture = new Texture2D();
    // entity.texture->isLoadedGPU = false;
    // entity.color = {1.0f, 1.0f, 1.0f, 1.0f};

    render_command load_texture;
    load_texture.type = UPLOAD;
    load_texture.filePath = "assets/textures/FD_Character_007_Idle.png";
    load_texture.texture = entity.sprite.texture;
    render_state->render_commands.push(load_texture);

    state->entities.push_back(entity);

    state->selected_entity = 0;
}
