#pragma once

#include <queue>
#include "entity.hpp"

enum RENDER_TYPE
{
    UPLOAD,
    DRAW_TEXTURE,
    COMPILE_SHADER,
};

struct vertex
{
    Vector2 position;
    Vector2 texCoords;
    Vector4 color;
};

struct render_shader
{
    unsigned int ID;
    const char *vertexPath;
    const char *fragmentPath;
};

struct render_camera
{
    Vector2 position;
    float rotation;
    float zoom;
};

struct render_framebuffer
{
    unsigned int fbo;
    unsigned int texture;
    unsigned int rbo;
    float width;
    float height;
};

struct render_command
{
    RENDER_TYPE type;
    uint32_t id;

    const char *filePath;

    unsigned int offset; // The position of the first vertices that is going to be drawn in the render state vertices
    unsigned int count; // Count of indices drawn
    Entity *entity;
    union {
        Texture2D *texture;
        render_shader *shader;
    };
    Vector4 color;
};



struct render_context
{
    std::queue<render_command> render_commands;
    unsigned int quadVAO;
    unsigned int VAO;
    unsigned int EBO;
    unsigned int VBO;
    unsigned int UVO;
    vertex vertices[4096];
    unsigned int vertexCount;
    unsigned int indices[4096 * 3];
    unsigned int indexCount;
    Matrix projection;
    render_shader shader;
    render_camera *camera;
    render_framebuffer *fbo;
    float width;
    float height;
    unsigned int defaultWhiteTexture;

    Vector3 color;
};

void draw_scene(render_context *context);

render_shader compile_shader(const char *vertexSource, const char *fragmentSource);

Matrix create_projection(float zoom, Vector2 position, float rotation, float width, float height);

void create_framebuffer(render_framebuffer *fbo, const float width, const float height);
void bind_framebuffer(render_framebuffer *fbo, float width, float height);
void unbind_framebuffer();
void delete_framebuffer(render_framebuffer *fbo);
