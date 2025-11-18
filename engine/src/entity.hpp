#pragma once

struct Transform
{
    Vector2 position;
    Vector2 scale;
    Vector2 size;
    float rotation = 0.0f;
};

struct Sprite
{
    Texture2D *texture;
    Vector4 color;
    int z_index = 0;
    bool flip_x;
    bool flip_y;
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
};

struct Entity
{
    unsigned int id;
    //Texture2D *texture;
    Transform transform;
    Sprite sprite;
    AnimatedSprite animated_sprite;
    char *name;
    size_t vertexOffset = 0; // index into the shared vertex buffer
    // glm::vec2 velocity;
    
    // Vector4 color;
};
