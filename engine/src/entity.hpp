#pragma once

typedef uint32_t flag_t;
enum
{
    ENTITY_SPRITE =          (1 << 0),
    ENTITY_ANIMATED_SPRITE = (1 << 1),
    ENTITY_TILEMAP =         (1 << 2),
};

struct Transform
{
    Vector2 position;
    Vector2 scale;
    Vector2 size; // Base size
    float rotation = 0.0f;
};

struct Sprite
{
    Texture2D *texture;
    Vector4 color;
    int z_index = 0;
    bool flip_x = false;
    bool flip_y = false;
};

struct AnimatedSprite
{
    int cell_width, cell_height;
    int texture_width, texture_height;
    int frame_index;
    int start_frame;
    int end_frame;
    int max_frames;
    float frame_duration;
    float current_duration;
    float offset_x;
    float offset_y;
    bool changed = false;
};

struct Tile
{
    unsigned int id;
    Sprite sprite;
    Transform transform;
    float u0 = 0;
    float u1 = 0;
    float v0 = 0;
    float v1 = 0;
    int index;
};

struct Tilemap
{
    unsigned int id;
    Sprite sprite;
    int cell_width = 8;
    int cell_height = 8;
    int texture_width, texture_height;
    std::vector<Tile> tiles;
    std::vector<Tile> map;
    bool enabled = false;
};

struct Entity
{
    unsigned int id;
    const char *name;
    size_t vertexOffset = 0; // index into the shared vertex buffer

    struct Entity *parent;
    struct Entity *children;
    Transform transform; // TODO: delete
    Transform local; // Compute Wt
    Transform world; // Compute vertices
    bool dirty; // If transform has changed, change to true

    Sprite sprite;
    AnimatedSprite animated_sprite;
    Tilemap tilemap;

    flag_t flags;
};
