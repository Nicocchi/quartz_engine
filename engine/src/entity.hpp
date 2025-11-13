#pragma once

struct Transform
{
    Vector2 position;
    Vector2 scale;
    Vector2 size;
    float rotation = 0.0f;
};

struct Entity
{
    unsigned int id;
    Texture2D *texture;
    Transform transform;
    char *name;
    size_t vertexOffset = 0; // index into the shared vertex buffer
    // glm::vec2 velocity;
    int width, height;
    int texture_width, texture_height;
    int frameIndex;
    int startFrame;
    int endFrame;
    int maxFrames;
    float frameDuration;
    float texture_offsetX;
    float texture_offsetY;
    Vector4 color;
};