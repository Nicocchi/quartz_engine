#include "game.hpp"
#include <random>
#include <cstdio>

// Layer - z-index [X]
//  don't change original order
//  secondary list of pointers to each entity and sort that based on z-index (per frame - discard after)
//  generate render commands

// blending
//  full blend mode - normal - additive - multiply - screen
//  opacity - transparency [X]

// texture subregions - texture atlas [X]

// flip texture (horz - vert) [x]

// gradients

// per-vertex coloring [x]
// per-vertex upload colors (only corners) - inbetween gets interpolated

// shapes
// lines - circles - points - mesh (basically a set of triangles) - can be deformed

// Lighting
// batch rend


// input [x]
// sound
// font
// physics - collisions - box2d
// animation
// state machines
// in-game ui
// profiling - perf/tracy (casey)
// threading
// hot reloading [x]

// tilemaps

void game_init(game_memory *GameMemory);

// bool compareEntities(const Entity *a, const Entity *b)
// {
//     return a->sprite.z_index < b->sprite.z_index;
// }

bool compareEntities(const Entity *a, const Entity *b)
{
    int az = a->tilemap.enabled ? a->tilemap.sprite.z_index : a->sprite.z_index;
    int bz = b->tilemap.enabled ? b->tilemap.sprite.z_index : b->sprite.z_index;
    return az < bz;
}

bool isMouseButtonPressed(unsigned int button, input_state *Input)
{
    if (button >= 32)
    {
        return false;
    }

    return Input->mouse_buttons[button];
}

bool isKeyPressed(unsigned int keycode, input_state *Input)
{
    if (keycode >= 1024)
    {
        return false;
    }

    return Input->keys[keycode];
}

unsigned int previous_keycode;

void process_input(render_context *render_state, game_state *state, input_state *Input, float dt)
{
    float velocity = 300 * dt;
    if (isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT, Input))
    {

    }

    if (isKeyPressed(GLFW_KEY_A, Input))
    {

        state->entities[0].transform.position.x -= velocity;
        state->entities[0].animated_sprite.start_frame = 8;
        state->entities[0].animated_sprite.end_frame = 11;
        if (previous_keycode != GLFW_KEY_A && !state->entities[0].animated_sprite.changed)
        {
            previous_keycode = GLFW_KEY_A;
            state->entities[0].animated_sprite.changed = true;
        }
        if (state->entities[0].animated_sprite.changed)
        {
            state->entities[0].animated_sprite.frame_index = 8;
            state->entities[0].animated_sprite.changed = false;
        }
        state->entities[0].dirty = true;
    }

    if (isKeyPressed(GLFW_KEY_D, Input))
    {
        state->entities[0].transform.position.x += velocity;
        state->entities[0].animated_sprite.start_frame = 4;
        state->entities[0].animated_sprite.end_frame = 7;

        if (previous_keycode != GLFW_KEY_D && !state->entities[0].animated_sprite.changed)
        {
            previous_keycode = GLFW_KEY_D;
            state->entities[0].animated_sprite.changed = true;
        }
        if (state->entities[0].animated_sprite.changed)
        {
            state->entities[0].animated_sprite.frame_index = 4;
            state->entities[0].animated_sprite.changed = false;
        }
        state->entities[0].dirty = true;
    }

    if (isKeyPressed(GLFW_KEY_W, Input))
    {
        state->entities[0].transform.position.y += velocity;
        state->entities[0].animated_sprite.start_frame = 0;
        state->entities[0].animated_sprite.end_frame = 3;

        if (previous_keycode != GLFW_KEY_W && !state->entities[0].animated_sprite.changed)
        {
            previous_keycode = GLFW_KEY_W;
            state->entities[0].animated_sprite.changed = true;
        }
        if (state->entities[0].animated_sprite.changed)
        {
            state->entities[0].animated_sprite.frame_index = 0;
            state->entities[0].animated_sprite.changed = false;
        }
        state->entities[0].dirty = true;
    }

    if (isKeyPressed(GLFW_KEY_S, Input))
    {
        state->entities[0].transform.position.y -= velocity;
        state->entities[0].animated_sprite.start_frame = 12;
        state->entities[0].animated_sprite.end_frame = 14;

        if (previous_keycode != GLFW_KEY_S && !state->entities[0].animated_sprite.changed)
        {
            previous_keycode = GLFW_KEY_S;
            state->entities[0].animated_sprite.changed = true;
        }
        if (state->entities[0].animated_sprite.changed)
        {
            state->entities[0].animated_sprite.frame_index = 12;
            state->entities[0].animated_sprite.changed = false;
        }
        state->entities[0].dirty = true;
    }

    // render_state->camera->position.x = state->entities[0].transform.position.x;
    // render_state->camera->position.y = state->entities[0].transform.position.y;
    // Smooth follow
    render_state->camera->position += (state->entities[0].transform.position - render_state->camera->position) * 10.0f * dt;
}

std::vector<Entity*> entities;
extern "C" __declspec(dllexport) GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    game_state *state = (game_state*)GameMemory->storage;
    render_context *render_state = (render_context*)GameMemory->renderStorage;


    if (!GameMemory->isInit)
    {
        game_init(GameMemory);
        GameMemory->isInit = true;
    }

    process_input(render_state, state, Input, deltaTime);



    entities.clear();
    for (int i = 0; i < state->entities.size(); i++)
    {
        entities.push_back(&state->entities[i]);
    }
    std::sort(entities.begin(), entities.end(), compareEntities);

    for (int i = 0; i <entities.size(); i++)
    {
        if (!entities[i]->tilemap.enabled)
        {
            state->entities[i].dirty = false;

            render_command draw_texture;
            draw_texture.type = DRAW_TEXTURE;
            draw_texture.id = entities[i]->id;
            draw_texture.texture = entities[i]->sprite.texture;

            Vector2 pos = entities[i]->transform.position;
            Vector2 esize = entities[i]->transform.size;
            float rotation = entities[i]->transform.rotation;

            vertex *v = &render_state->vertices[render_state->vertexCount];

            // Sprite animation
            float tw = float(entities[i]->animated_sprite.cell_width) / entities[i]->animated_sprite.texture_width;
            float th = float(entities[i]->animated_sprite.cell_height) / entities[i]->animated_sprite.texture_height;
            int width = entities[i]->animated_sprite.cell_width;
            int twidth = entities[i]->animated_sprite.texture_width;
            int numPerRow = twidth / width;

            entities[i]->animated_sprite.current_duration += deltaTime;



            if (entities[i]->animated_sprite.current_duration >= entities[i]->animated_sprite.frame_duration)
            {
                entities[i]->animated_sprite.current_duration = 0.0f;


                entities[i]->animated_sprite.frame_index++;

                if (entities[i]->animated_sprite.frame_index > entities[i]->animated_sprite.end_frame)
                {
                    entities[i]->animated_sprite.frame_index = entities[i]->animated_sprite.start_frame;
                }
            }

            int frame = entities[i]->animated_sprite.frame_index;

            float tx = (frame % numPerRow) * tw;
            float ty = 1.0f - ((frame / numPerRow) + 1) * th;

            tx += entities[i]->animated_sprite.offset_x;
            ty += entities[i]->animated_sprite.offset_y;

            float u0 = tx;
            float u1 = tx + tw;
            float v0 = ty;
            float v1 = ty + th;

            if (entities[i]->sprite.flip_x)
            {
                std::swap(u0, u1);
            }

            if (entities[i]->sprite.flip_y)
            {
                std::swap(v0, v1);
            }

            v[0].position = pos;
            // Top-left
            v[0].texCoords = {u0, v1};
            v[0].color.x = entities[i]->sprite.color.x;
            v[0].color.y = entities[i]->sprite.color.y;
            v[0].color.z = entities[i]->sprite.color.z;
            v[0].color.w = entities[i]->sprite.color.w;

            v[1].position = {pos.x + esize.x, pos.y};
            // Top-right
            v[1].texCoords = {u1, v1};
            v[1].color.x = entities[i]->sprite.color.x;
            v[1].color.y = entities[i]->sprite.color.y;
            v[1].color.z = entities[i]->sprite.color.z;
            v[1].color.w = entities[i]->sprite.color.w;

            v[2].position = {pos.x + esize.x, pos.y + esize.y};
            // Bottom-right
            v[2].texCoords = {u1, v0};
            v[2].color.x = entities[i]->sprite.color.x;
            v[2].color.y = entities[i]->sprite.color.y;
            v[2].color.z = entities[i]->sprite.color.z;
            v[2].color.w = entities[i]->sprite.color.w;

            v[3].position = {pos.x, pos.y + esize.y};
            // Bottom-left
            v[3].texCoords = {u0, v0};
            v[3].color.x = entities[i]->sprite.color.x;
            v[3].color.y = entities[i]->sprite.color.y;
            v[3].color.z = entities[i]->sprite.color.z;
            v[3].color.w = entities[i]->sprite.color.w;


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
        else
        {
            for (int t = 0; t < entities[i]->tilemap.map.size(); t++)
            {
                Tile mtile = entities[i]->tilemap.map[t];

                render_command draw_texture;
                draw_texture.type = DRAW_TEXTURE;
                draw_texture.id = mtile.id;
                draw_texture.texture = entities[i]->tilemap.sprite.texture;

                Vector2 pos = mtile.transform.position;
                Vector2 esize = mtile.transform.size;
                float rotation = mtile.transform.rotation;

                vertex *v = &render_state->vertices[render_state->vertexCount];

                float u0 = mtile.u0;
                float u1 = mtile.u1;
                float v0 = mtile.v0;
                float v1 = mtile.v1;

                // Top-left vertex
                v[0].position = pos;
                v[0].texCoords = {u0, v1};
                v[0].color = entities[i]->tilemap.sprite.color;

                // Top-right vertex
                v[1].position = {pos.x + esize.x, pos.y};
                v[1].texCoords = {u1, v1};
                v[1].color = entities[i]->tilemap.sprite.color;

                // Bottom-right vertex
                v[2].position = {pos.x + esize.x, pos.y + esize.y};
                v[2].texCoords = {u1, v0};
                v[2].color = entities[i]->tilemap.sprite.color;

                // Bottom-left vertex
                v[3].position = {pos.x, pos.y + esize.y};
                v[3].texCoords = {u0, v0};
                v[3].color = entities[i]->tilemap.sprite.color;

                // Handle rotation if needed
                if (rotation != 0)
                {
                    float c = cos(rotation);
                    float s = sin(rotation);
                    Vector2 center = pos + esize / 2;

                    for (int j = 0; j < 4; j++)
                    {
                        Vector2 p = v[j].position;
                        float offset_x = p.x - center.x;
                        float offset_y = p.y - center.y;
                        float rx = offset_x * c - offset_y * s;
                        float ry = offset_x * s + offset_y * c;
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
    }
}

void game_init(game_memory *GameMemory)
{

    game_state *state = (game_state*)GameMemory->storage;
    render_context *render_state = (render_context*)GameMemory->renderStorage;

    state->entities.clear();
    state->entities_sorted.clear();
    state->selected_entity = 0;

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


    Entity entity;
    entity.id = state->nextID++;
    entity.name = "Player";
    entity.transform.position.x = -91;
    entity.transform.position.y = -91;
    entity.transform.size.x = 240.0f;
    entity.transform.size.y = 240.0f;
    entity.transform.rotation = 0.0f;
    entity.dirty = true;

    entity.flags = ENTITY_SPRITE | ENTITY_ANIMATED_SPRITE;

    entity.sprite.texture = new Texture2D();
    entity.sprite.texture->isLoadedGPU = false;
    entity.sprite.color = {1.0f, 1.0f, 1.0f, 1.0f};
    entity.sprite.z_index = 1;

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

    entity.tilemap.enabled = false;

    render_command load_texture;
    load_texture.type = UPLOAD;
    load_texture.filePath = "assets/textures/FD_Character_007_Idle.png";
    load_texture.texture = entity.sprite.texture;
    render_state->render_commands.push(load_texture);



    state->entities.push_back(entity);

    state->selected_entity = 0;

    Entity entity2;
    entity2.id = state->nextID++;
    entity2.name = "Tilemap";
    entity2.transform.position.x = -91;
    entity2.transform.position.y = -91;
    entity2.transform.size.x = 240.0f;
    entity2.transform.size.y = 240.0f;
    entity2.transform.rotation = 0.0f;
    entity2.dirty = true;

    entity2.flags = ENTITY_TILEMAP;

    entity2.tilemap.sprite.texture = new Texture2D();
    entity2.tilemap.sprite.texture->isLoadedGPU = false;
    entity2.tilemap.sprite.color = {1.0f, 1.0f, 1.0f, 1.0f};
    entity2.tilemap.sprite.z_index = 0;
    entity2.tilemap.id = state->nextID++;
    entity2.tilemap.texture_width = 96;
    entity2.tilemap.texture_height = 192;
    entity2.tilemap.cell_width = 16;
    entity2.tilemap.cell_height = 16;
    entity2.tilemap.enabled = true;

    render_command load_tilemap_texture;
    load_tilemap_texture.type = UPLOAD;
    load_tilemap_texture.filePath = "assets/textures/overworld.png"; // Change to your tilemap texture path
    load_tilemap_texture.texture = entity2.tilemap.sprite.texture;
    render_state->render_commands.push(load_tilemap_texture);

    int cellW = entity2.tilemap.cell_width;
    int cellH = entity2.tilemap.cell_height;
    int texW = entity2.tilemap.texture_width;
    int texH = entity2.tilemap.texture_height;

    int tilesX = texW / cellW;
    int tilesY = texH / cellH;
    int tileCount = tilesX * tilesY;

    float uvW = (float)cellW / texW;
    float uvH = (float)cellH / texH;

    for (int i = 0; i < tileCount; i++)
    {
        if (i == 3 || i == 4 || i == 5 || i == 10 || i == 11 || i == 17 || i == 23)
        {
            continue;
        }
        int x = i % tilesX;
        int y = i / tilesX;

        float u0 = x * uvW;
        float v0 = y * uvH;
        float u1 = u0 + uvW;
        float v1 = v0 + uvH;
        Tile tile;
        tile.id = state->nextID++;
        tile.u0 = u0;
        tile.v0 = v0;
        tile.u1 = u1;
        tile.v1 = v1;
        entity2.tilemap.tiles.push_back(tile);
    }

    Vector2 pos = { -2400.0f, 1600.0f };
    for (int i = 0; i < 1000; i++)
    {
        Tile tiled;
        tiled.id = state->nextID++;
        tiled.transform.position = pos;
        float size = 273.0f;
        pos.x += size;
        if (pos.x > 2400.0f)
        {
            pos.x = -2400.0f;
            pos.y -= size;
        }
        tiled.transform.size.x = size;
        tiled.transform.size.y = size;
        int min = 0;
        int max = 9;
        int randidx = (rand() % (max - min + 1)) + min;
        tiled.u0 = entity2.tilemap.tiles[randidx].u0;
        tiled.v0 = entity2.tilemap.tiles[randidx].v0;
        tiled.u1 = entity2.tilemap.tiles[randidx].u1;
        tiled.v1 = entity2.tilemap.tiles[randidx].v1;
        tiled.index = randidx;
        entity2.tilemap.map.push_back(tiled);
    }

    state->entities.push_back(entity2);
}
