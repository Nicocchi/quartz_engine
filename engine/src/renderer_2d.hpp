#pragma once

#include <queue>

#include "raymath_ext.h"

#include "texture_2d.hpp"
#include "entity.hpp"
#include "platform.hpp"
#include "logger.hpp"

enum class RENDER_TYPE
{
    UPLOAD,
    COMPILE_SHADER,
    DRAW_TEXTURE,
    DRAW_LINE,
};

struct Vertex
{
    Vector2 position;
    Vector2 texCoords;
    Vector4 color;
};

struct Shader
{
    unsigned int ID = 0;
    const char *vertexPath;
    const char *fragmentPath;
};

struct Camera
{
    Vector2 position;
    float rotation;
    float zoom;
};

struct GLFramebuffer
{
    unsigned int fbo;
    unsigned int texture;
    unsigned int rbo;
    float width;
    float height;
};

struct RenderCommand
{
    RENDER_TYPE type;

    const char *filePath;

    // The position of the first vertices that is 
    // going to be drawn in the render state vertices
    unsigned int offset;
    unsigned int count; // Count of indices drawn
    Entity *entity;
    union {
        Texture2D *texture;
        Shader *shader;
    };
    Vector4 color;
};

struct RenderEditorCommand
{
    RENDER_TYPE type;

    const char *filePath;

    // The position of the first vertices that is 
    // going to be drawn in the render state vertices
    unsigned int offset;
    unsigned int count; // Count of indices drawn
    Entity *entity;
    union {
        Texture2D *texture;
        Shader *shader;
    };
    Vector2 line_pos_a;
    Vector2 line_pos_b;
};

struct RenderContext
{
    std::queue<RenderCommand> commands;
    std::queue<RenderEditorCommand> editor_render_commands;
    unsigned int quadVAO;
    unsigned int VAO;
    unsigned int EBO;
    unsigned int VBO;
    unsigned int UVO;

    unsigned int lineVAO;
    unsigned int lineVBO;
    float lineVertices[4096 * 2 *3]; // 2 points * 3 floats per line
    unsigned int lineVertexCount;
    Vector4 lineColor;

    Vertex vertices[4096];
    unsigned int vertexCount;
    unsigned int indices[4096 * 6];
    unsigned int indexCount;

    Matrix projection;

    Shader shader;      // Quad
    Shader lineShader;  // Line
    int lineShader_projectionLoc;
    int lineShader_colorLoc;
    int quad_projectionLoc;

    Camera *camera;
    GLFramebuffer *fbo;
    
    float width;
    float height;
    
    unsigned int defaultWhiteTexture;

    Vector3 color;
};

void InitRenderer(RenderContext *context, const float width, const float height, EngineLogger *engineLogger);

void DrawBatchedScene(RenderContext *context, EngineLogger *engineLogger);

Shader CompileShader(const char *vertexSource, const char *fragmentSource, EngineLogger *engineLogger);

Matrix CreateProjection(float zoom, Vector2 position, float rotation, float width, float height);

void CreateFramebuffer(GLFramebuffer *fbo, const float width, const float height);
void BindFramebuffer(GLFramebuffer *fbo, float width, float height);
void DrawFramebuffer(GLFramebuffer *fbo, float width, float height);
void UnbindFramebuffer();
void DeleteFramebuffer(GLFramebuffer *fbo);

void RenderEntities(game_memory *GM, float deltaTime);