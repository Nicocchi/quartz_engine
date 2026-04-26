#include "renderer_2d.hpp"
#include "logger.hpp"
#include "platform.hpp"
#include <fstream>

#include "game.hpp"
#include "../vendor/glad/glad.h"
#include "texture_2d.hpp"

/**
 * @brief Gets the OpenGL error number and prints out the provided
 *          user message.
 * 
 * @param message - User message
 * @param engineLogger - Core engine logger
 */
void CheckGLError(const char *message, EngineLogger *engineLogger)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "GL error 0x%x at: %s", err, message);
        std::string msg = buf;
        printf("GL error 0x%x at: %s\n", err, message);
        EngineLog log;
        log.type = LogType::SYSTEM;
        log.messages.push_back((msg));
        log.severity = LogSeverity::HIGH;
        engineLogger->logs.push_back(log);
    }
}

/**
 * @brief Creates the VAO and VBO, allocates the buffer, and defines the vertex layout
 *        for the editor's grid lines.
 * 
 * @param context 
 */
void InitLineBatch(RenderContext *context)
{
    glGenVertexArrays(1, &context->lineVAO);
    glGenBuffers(1, &context->lineVBO);

    glBindVertexArray(context->lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, context->lineVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(context->lineVertices), nullptr, 
                GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                    3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

/**
 * @brief Initializes the renderer. Both Quad and Editor grid lines
 *           default shader and locations
 * 
 * @param context 
 * @param width 
 * @param height 
 */
void InitRenderer(RenderContext *context, const float width,
                    const float height, EngineLogger *engineLogger)
{
    context->shader = CompileShader("assets/shaders/basic.vs", 
                                    "assets/shaders/basic.frag", engineLogger);
    context->lineShader = CompileShader("assets/shaders/line.vs", 
                                        "assets/shaders/line.frag", engineLogger);

    context->lineShader_projectionLoc = glGetUniformLocation(context->lineShader.ID, "projection");
    context->lineShader_colorLoc = glGetUniformLocation(context->lineShader.ID, "lineColor");
    context->quad_projectionLoc = glGetUniformLocation(context->shader.ID, "projection");

    // Quad gemotry
    GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f, // Bottom-left
        0.0f, 1.0f, 0.0f, // Top-left
        1.0f, 1.0f, 0.0f, // Top-right
        1.0f, 0.0f, 0.0f  // Bottom-right
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    GLfloat uv_data[] = {
        0.0f, 1.0f,  // Bottom-Left
        0.0f, 0.0f,  // Top-Left
        1.0f, 0.0f,  // Top-Right
        1.0f, 1.0f   // Bottom-Right
    };

    GLuint pos_index = glGetAttribLocation(context->shader.ID, "pos");
    GLuint uv_index = glGetAttribLocation(context->shader.ID, "uv");
    GLuint color_index = glGetAttribLocation(context->shader.ID, "aColor");

    // --- Setup VAO/VBO/EBO ---
    glGenVertexArrays(1, &context->VAO);
    glGenBuffers(1, &context->VBO);
    glGenBuffers(1, &context->EBO);
    glGenBuffers(1, &context->UVO);

    glBindVertexArray(context->VAO);


    // Element indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 4096 * 6,
            nullptr, GL_DYNAMIC_DRAW);

    // Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, context->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4096, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(pos_index, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // UVs
    glVertexAttribPointer(uv_index, 2, GL_FLOAT, GL_FALSE, 
                sizeof(Vertex), (void*)8);
    glEnableVertexAttribArray(1);

    // Color
    glVertexAttribPointer(color_index, 4, GL_FLOAT, GL_FALSE, 
                sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);

    // Create Framebuffer
    context->fbo = new GLFramebuffer();
    CreateFramebuffer(context->fbo, width, height);

    // Create a 1x1 white pixel texture
    unsigned int whiteTex;
    unsigned char whitePixel[4] = { 255, 255, 255, 255 }; // RGBA

    glGenTextures(1, &whiteTex);
    glBindTexture(GL_TEXTURE_2D, whiteTex);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA,
        1, 1, 0,
        GL_RGBA, GL_UNSIGNED_BYTE,
        whitePixel
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    context->defaultWhiteTexture = whiteTex;

    // Initialize the editor grid lines
    InitLineBatch(context);
}

/**
 * @brief Draws a batch of quads with the same texture
 * 
 * @param context 
 * @param textureToBind 
 * @param indexCount 
 * @param indexStart 
 * @param engineLogger 
 */
void Flush(RenderContext *context, unsigned int textureToBind, int indexCount,
            int indexStart, EngineLogger *engineLogger)
{
    if (indexCount == 0) return;
    if (context->shader.ID == 0) return;

    glUseProgram(context->shader.ID);
    CheckGLError("flush: after glUseProgram", engineLogger);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(context->VAO);
    glBindTexture(GL_TEXTURE_2D, textureToBind);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT,
        (void*)(indexStart * sizeof(unsigned int)));

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


/**
 * @brief Sends all the editor grid lines to the GPU in one draw call.
 * 
 * @param context 
 * @param engineLogger 
 * @param color 
 */
void FlushLines(RenderContext *context, EngineLogger *engineLogger, 
                Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f})
{
    if (context->lineVertexCount == 0) return;
    if (context->lineShader.ID == 0) return;

    glUseProgram(context->lineShader.ID);
    CheckGLError("flush_lines: after glUseProgram", engineLogger);

    glUniformMatrix4fv(context->lineShader_projectionLoc, 1, GL_FALSE,
                        (const GLfloat*)&context->projection);
    CheckGLError("flush_lines: after projection uniform", engineLogger);

    glUniform4f(context->lineShader_colorLoc,
                color.x, color.y, color.z, color.w);

    glBindVertexArray(context->lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, context->lineVBO);

    glBufferSubData(GL_ARRAY_BUFFER, 0, context->lineVertexCount * 3 * sizeof(float),
                    context->lineVertices);

    glDrawArrays(GL_LINES, 0, context->lineVertexCount);

    glBindVertexArray(0);
    context->lineVertexCount = 0;
}

/**
 * @brief Adds an editor grid line to the CPU buffer. 2 vertices per line.
 * 
 * @param context 
 * @param command 
 */
void StageGridLines(RenderContext *context, RenderEditorCommand *command)
{
    unsigned int base = context->lineVertexCount * 3;

    context->lineVertices[base + 0] = command->line_pos_a.x;
    context->lineVertices[base + 1] = command->line_pos_a.y;
    context->lineVertices[base + 2] = 0.0f;

    context->lineVertices[base + 3] = command->line_pos_b.x;
    context->lineVertices[base + 4] = command->line_pos_b.y;
    context->lineVertices[base + 5] = 0.0f;

    context->lineVertexCount += 2; // 2 vertices per line
}

/**
 * @brief Draws the game screen. Loops through all the render commands, allocating them
 *         into buffers and renders them in one draw call.
 * 
 * @param context 
 * @param engineLogger 
 */
void DrawBatchedScene(RenderContext *context, EngineLogger *engineLogger)
{
    glBindBuffer(GL_ARRAY_BUFFER, context->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * context->vertexCount, 
            context->vertices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, 
            sizeof(unsigned int) * context->indexCount, context->indices);

    unsigned int textureToBind = context->defaultWhiteTexture;
    unsigned int shader = context->shader.ID;

    int indexStart = 0;
    int indexCount = 0;
    for (; !context->commands.empty(); context->commands.pop())
    {
        RenderCommand command = context->commands.front();

        switch(command.type)
        {
            // Create's a texture for the entity with the given filepath
            case RENDER_TYPE::UPLOAD:
            {
                if (!command.texture->isLoadedGPU) LoadTexture(command.texture, command.filePath);
            } break;

            // Creates a shader
            case RENDER_TYPE::COMPILE_SHADER:
            {
                context->shader = CompileShader(command.shader->vertexPath,
                                                command.shader->fragmentPath, engineLogger);
                context->quad_projectionLoc = glad_glGetUniformLocation(context->shader.ID, 
                                                                "projection");
            } break;

            // Batches the draw calls into one draw call
            case RENDER_TYPE::DRAW_TEXTURE:
            {
                // If first run, use the quad shader
                if (indexCount == 0 && indexStart == 0)
                {
                    glUseProgram(context->shader.ID);
                    CheckGLError("after glUseProgram quad shader", engineLogger);
                    glUniformMatrix4fv(context->quad_projectionLoc, 1, GL_FALSE,
                                        (const GLfloat*)&context->projection);
                    CheckGLError("after quad projection uniform", engineLogger);
                }
                GLuint tex = (command.texture && command.texture->isLoadedGPU) ?
                                command.texture->ID : context->defaultWhiteTexture;

                // If texture changes, flush
                if (tex != textureToBind && indexCount > 0)
                {
                    Flush(context, textureToBind, indexCount, indexStart, engineLogger);
                    indexStart += indexCount;
                    indexCount = 0;
                }

                // Rebind texture and accumulate indices
                textureToBind = tex;
                indexCount += command.count;
            } break;
            case RENDER_TYPE::DRAW_LINE:
              break;
            }
    }

    for (; !context->editor_render_commands.empty(); context->editor_render_commands.pop())
    {
        RenderEditorCommand command = context->editor_render_commands.front();

        switch(command.type)
        {
            case RENDER_TYPE::DRAW_LINE:
            {
                StageGridLines(context, &command);
            } break;

            case RENDER_TYPE::UPLOAD:
            case RENDER_TYPE::COMPILE_SHADER:
            case RENDER_TYPE::DRAW_TEXTURE:
              break;
            }
    }

    Flush(context, textureToBind, indexCount, indexStart, engineLogger);
    FlushLines(context, engineLogger, context->lineColor);
    context->vertexCount = 0;
    context->indexCount = 0;
    indexStart = 0;
}

/**
 * Compare entities based on z-index and texture ID
 */
bool compareEntities(const Entity *a, const Entity *b)
{
    // Z-index
    if (a->sprite.z_index != b->sprite.z_index)
        return a->sprite.z_index < b->sprite.z_index;

    // Same z-index, compare by texture instead
    return a->sprite.texture->ID < b->sprite.texture->ID;
}

std::vector<Entity*> entities;

/**
 * @brief Loops through a list of entities, sorts them based on z-index,
 *          creates the vertices for size, scale, position, color, texture
 *          and then creates RenderCommands and pushes them into the
 *          render queue.
 * 
 * @param GM 
 * @param deltaTime 
 */
void RenderEntities(game_memory *GM, float deltaTime)
{
    game_state *state = (game_state*)GM->storage;
    RenderContext *render_state = (RenderContext*)GM->renderStorage;

    entities.clear();
    if (state->scenes.size() <= 0)
    {
        return;
    }
    for (int i = 0; i < state->scenes[state->current_scene].entities.size(); i++)
    {
        entities.push_back(&state->scenes[state->current_scene].entities[i]);
    }
    std::sort(entities.begin(), entities.end(), compareEntities);

    for (int i = 0; i < entities.size(); i++)
    {
        if (!entities[i]->tilemap.enabled)
        {
            entities[i]->dirty = false;

            RenderCommand draw_texture;
            draw_texture.type = RENDER_TYPE::DRAW_TEXTURE;
            draw_texture.texture = entities[i]->sprite.texture;

            Vector2 pos = entities[i]->transform.position;
            Vector2 esize = entities[i]->transform.size * entities[i]->transform.scale;
            float rotation = entities[i]->transform.rotation;

            Vertex *v = &render_state->vertices[render_state->vertexCount];

            // Sprite animation
            float tw = float(entities[i]->animated_sprite.cell_width) /
                            entities[i]->animated_sprite.texture_width;
            float th = float(entities[i]->animated_sprite.cell_height) /
                            entities[i]->animated_sprite.texture_height;

                            int width = entities[i]->animated_sprite.cell_width;
            int twidth = entities[i]->animated_sprite.texture_width;
            int numPerRow = twidth / width;

            entities[i]->animated_sprite.current_duration += deltaTime;

            if (entities[i]->animated_sprite.current_duration >=
                entities[i]->animated_sprite.frame_duration)
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

            render_state->commands.push(draw_texture);
        }
        else
        {
            for (auto& pair : entities[i]->tilemap.map)
            {
                Tile &mtile = pair.second;

                RenderCommand draw_texture;
                draw_texture.type = RENDER_TYPE::DRAW_TEXTURE;
                draw_texture.texture = entities[i]->sprite.texture;

                Vector2 pos = mtile.transform.position;
                Vector2 esize = mtile.transform.size * mtile.transform.scale;
                float rotation = mtile.transform.rotation;

                Vertex *v = &render_state->vertices[render_state->vertexCount];

                float u0 = mtile.u0;
                float u1 = mtile.u1;
                float v0 = mtile.v0;
                float v1 = mtile.v1;

                // Top-left vertex
                v[0].position = pos;
                v[0].texCoords = {u0, v1};
                v[0].color = entities[i]->sprite.color;

                // Top-right vertex
                v[1].position = {pos.x + esize.x, pos.y};
                v[1].texCoords = {u1, v1};
                v[1].color = entities[i]->sprite.color;

                // Bottom-right vertex
                v[2].position = {pos.x + esize.x, pos.y + esize.y};
                v[2].texCoords = {u1, v0};
                v[2].color = entities[i]->sprite.color;

                // Bottom-left vertex
                v[3].position = {pos.x, pos.y + esize.y};
                v[3].texCoords = {u0, v0};
                v[3].color = entities[i]->sprite.color;

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

                render_state->commands.push(draw_texture);
            }
        }
    }
}

/**
 * @brief Removes the GPU buffers
 * 
 * @param context 
 */
void DeleteScene(RenderContext *context)
{
    glDeleteVertexArrays(1, &context->VAO);
    glDeleteBuffers(1, &context->VBO);
    glDeleteBuffers(1, &context->EBO);
}

/**
 * @brief Prints shader compile/link errors
 * 
 * @param object 
 * @param type 
 */
void CheckCompileErrors(unsigned int object, std::string type, EngineLogger *engineLogger)
{
    int success;
    char infoLog[1024];
    EngineLog log;
    log.type = LogType::SYSTEM;
    log.severity = LogSeverity::HIGH;
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            char buf[256];
            snprintf(buf, sizeof(buf), "Shader Error: %s", infoLog);
            std::string msg = buf;
            log.messages.push_back((msg));
            printf("Shader: %s", infoLog);
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            char buf[256];
            snprintf(buf, sizeof(buf), "Program Error: %s", infoLog);
            std::string msg = buf;
            log.messages.push_back((msg));
            printf("Program: %s", infoLog);
        }
    }
}

// TODO (Nico): Refactor file loading
/**
 * @brief Reads the shader file into a string
 * 
 * @param filepath 
 * @return std::string 
 */
std::string ReadFile(const std::string& filepath) {
		std::ifstream ifs((filepath).c_str());
		std::string content(
			std::istreambuf_iterator<char>(ifs.rdbuf()),
			std::istreambuf_iterator<char>()
			);
		return content;
}

/**
* @brief Takes the given filepaths for vertex and fragment shaders
*           and loads them into string buffers, compiles them and
*           links them against the main GL program.
* 
* @param vPath 
* @param fPath 
* @return Shader 
*/
Shader CompileShader(const char *vPath, const char *fPath, EngineLogger *engineLogger)
{

    std::string vertexSourceS = ReadFile(vPath);
    std::string fragmentSourceS = ReadFile(fPath);
    const char *vertexSource = vertexSourceS.c_str();
    const char *fragmentSource = fragmentSourceS.c_str();

    Shader shader;
    unsigned int sVertex, sFragment;
    // Vertex
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    CheckCompileErrors(sVertex, "VERTEX", engineLogger);

    // Frag
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    CheckCompileErrors(sFragment, "FRAGMENT", engineLogger);

    // Shader program
    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, sVertex);
    glAttachShader(shader.ID, sFragment);

    glLinkProgram(shader.ID);
    CheckCompileErrors(shader.ID, "PROGRAM", engineLogger);

    glDeleteShader(sVertex);
    glDeleteShader(sFragment);

    return shader;
}

/**
 * @brief Builds a projection matrix from given params
 * 
 * @param zoom 
 * @param position 
 * @param rotation 
 * @param width 
 * @param height 
 * @return Matrix 
 */
Matrix CreateProjection(float zoom, Vector2 position, float rotation, float width, float height)
{

    float halfW = width / zoom;
    float halfH = height / zoom;

    Matrix view = MatrixIdentity();
    Matrix translate = view * MatrixTranslate(-position.x, -position.y, 0.0f);
    view = MatrixRotateZ(-rotation) * translate;
    Matrix projection = MatrixOrtho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
    view = MatrixTranspose(view);
    return projection * view;
}

/**
 * @brief Creates a framebuffer
 * 
 * @param fbo 
 * @param width 
 * @param height 
 */
void CreateFramebuffer(GLFramebuffer *fbo, const float width, const float height)
{
    fbo->width = width;
    fbo->height = height;
    glGenFramebuffers(1, &fbo->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);

    glGenTextures(1, &fbo->texture);
    glBindTexture(GL_TEXTURE_2D, fbo->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->texture, 0);

    glGenRenderbuffers(1, &fbo->rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
	                            fbo->rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is not complete");
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

/**
 * @brief Binds the framebuffer
 * 
 * @param fbo 
 * @param width 
 * @param height 
 */
void BindFramebuffer(GLFramebuffer *fbo, float width, float height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    glViewport(0, 0, width, height);
}

/**
 * @brief Unbinds the framebuffer
 * 
 */
void UnbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Draws the framebuffer to the screen
 * 
 * @param fbo 
 * @param width 
 * @param height 
 */
void DrawFramebuffer(GLFramebuffer *fbo, float width, float height)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, fbo->width, fbo->height, 0, 0, width, height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

/**
 * @brief Deletes/frees the framebuffer
 * 
 * @param fbo 
 */
void DeleteFramebuffer(GLFramebuffer *fbo)
{
    glDeleteFramebuffers(1, &fbo->fbo);
    glDeleteTextures(1, &fbo->texture);
    glDeleteRenderbuffers(1, &fbo->rbo);
}
