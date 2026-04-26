#pragma once

#include <stdint.h>
#include "texture_2d.hpp"
#include "raymath_ext.h"
#include "../vendor/uuid_v4.h"
#include <unordered_map>

typedef uint32_t flag_t;
enum
{
    ENTITY_SPRITE =          (1 << 0),
    ENTITY_ANIMATED_SPRITE = (1 << 1),
    ENTITY_TILEMAP =         (1 << 2),
};

struct GridCoord
{
    int col;
    int row;

    bool operator==(const GridCoord& other) const {
        return col == other.col && row == other.row;
    }
};

struct GridCoordHash
{
    std::size_t operator()(const GridCoord& c) const {
        // 64-bit mix
        uint64_t key = ((uint64_t)(uint32_t)c.col << 32) ^ (uint32_t)c.row;

        // SplitMix64 finalizer
        key ^= key >> 33;
        key *= 0xff51afd7ed558ccdULL;
        key ^= key >> 33;
        key *= 0xc4ceb9fe1a85ec53ULL;
        key ^= key >> 33;

        return (size_t)key;
    }

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
    std::string filename;
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
    int cell_width = 8;
    int cell_height = 8;
    int texture_width, texture_height;
    std::vector<Tile> tiles;
    std::unordered_map<GridCoord, Tile, GridCoordHash> map;
    bool enabled = false;
};

struct Entity
{
    unsigned int id;
    UUIDv4::UUID uuid;
    // const char *name;
    std::string name;
    unsigned int vertexOffset = 0; // index into the shared vertex buffer

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

struct Scene
{
    unsigned int id;
    UUIDv4::UUID uuid;
    std::string name;

    // Entity management
    std::vector<Entity> entities;
    std::vector<Entity*> entities_sorted;
    unsigned int selected_entity = 0;
    unsigned int selected_tile = -1;
};