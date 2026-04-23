#include "renderer_2d.hpp"
#include "platform.hpp"
#include <fstream>

#include "game.hpp"
#include "../vendor/glad/glad.h"

void init_renderer(render_context *context, const float width, const float height)
{

    context->shader = compile_shader("assets/shaders/basic.vs", "assets/shaders/basic.frag");

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
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // Vertex positions
    glBindBuffer(GL_ARRAY_BUFFER, context->VBO);
    glVertexAttribPointer(pos_index, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(0);


    // UVs
    glVertexAttribPointer(uv_index, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)8);
    glEnableVertexAttribArray(1);

    // Color
    glVertexAttribPointer(color_index, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);

    context->fbo = new render_framebuffer();
    create_framebuffer(context->fbo, width, height);

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

    // Optional: set standard filtering/wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    context->defaultWhiteTexture = whiteTex;
}

void draw_scene(render_context *context)
{
    // Upload (glBufferData)
    glBindBuffer(GL_ARRAY_BUFFER, context->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * context->vertexCount, context->vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * context->indexCount, context->indices, GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(glGetUniformLocation(context->shader.ID, "projection"), 1, GL_FALSE, (const GLfloat*)&context->projection);

    for (; !context->render_commands.empty(); context->render_commands.pop())
    {
        render_command command = context->render_commands.front();

        switch(command.type)
        {
            case UPLOAD:
            {
                if (!command.texture->isLoadedGPU)
                {
                    LoadTexture(command.texture, command.filePath);
                }

            } break;

            case DRAW_TEXTURE:
            {
                GLuint textureToBind = context->defaultWhiteTexture;

                if (command.texture && command.texture->isLoadedGPU)
                {
                    textureToBind = command.texture->ID;
                }

                glUseProgram(context->shader.ID);

                glActiveTexture(GL_TEXTURE0);

                glBindVertexArray(context->VAO);
                glBindTexture(GL_TEXTURE_2D, textureToBind);

                glDrawElementsBaseVertex(GL_TRIANGLES, command.count, GL_UNSIGNED_INT, 0, command.offset);
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);

            } break;

            case DRAW_LINE:
            {
                glUseProgram(context->shader.ID);

                float vertices[] = {
                    command.line_pos_a.x, command.line_pos_a.y, 0.0f,
                    command.line_pos_b.x, command.line_pos_b.y, 0.0f
                };

                glUseProgram(context->shader.ID);

                GLuint vao, vbo;
                glGenVertexArrays(1, &vao);
                glGenBuffers(1, &vbo);

                glBindVertexArray(vao);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glDrawArrays(GL_LINES, 0, 2);

                glDeleteVertexArrays(1, &vao);
                glDeleteBuffers(1, &vbo);
            } break;

            case COMPILE_SHADER:
            {
                context->shader = compile_shader(command.shader->vertexPath, command.shader->fragmentPath);
            } break;

        }
    }

    for (; !context->editor_render_commands.empty(); context->editor_render_commands.pop())
    {
        render_command command = context->editor_render_commands.front();

        switch(command.type)
        {
            case UPLOAD:
            case DRAW_TEXTURE:
            case COMPILE_SHADER:
            {
                return;
            } break;

            case DRAW_LINE:
            {
                glUseProgram(context->shader.ID);

                float vertices[] = {
                    command.line_pos_a.x, command.line_pos_a.y, 0.0f,
                    command.line_pos_b.x, command.line_pos_b.y, 0.0f
                };

                glUseProgram(context->shader.ID);

                GLuint vao, vbo;
                glGenVertexArrays(1, &vao);
                glGenBuffers(1, &vbo);

                glBindVertexArray(vao);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glDrawArrays(GL_LINES, 0, 2);

                glDeleteVertexArrays(1, &vao);
                glDeleteBuffers(1, &vbo);
            } break;

        }
    }
}

bool compareEntities(const Entity *a, const Entity *b)
{
    int az = a->tilemap.enabled ? a->sprite.z_index : a->sprite.z_index;
    int bz = b->tilemap.enabled ? b->sprite.z_index : b->sprite.z_index;
    return az < bz;
}

std::vector<Entity*> entities;
void render_entities(game_memory *GM, float deltaTime)
{
    game_state *state = (game_state*)GM->storage;
    render_context *render_state = (render_context*)GM->renderStorage;

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

            render_command draw_texture;
            draw_texture.type = DRAW_TEXTURE;
            draw_texture.texture = entities[i]->sprite.texture;

            Vector2 pos = entities[i]->transform.position;
            Vector2 esize = entities[i]->transform.size * entities[i]->transform.scale;
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
            for (auto& pair : entities[i]->tilemap.map)
            {
                Tile &mtile = pair.second;

                render_command draw_texture;
                draw_texture.type = DRAW_TEXTURE;
                draw_texture.texture = entities[i]->sprite.texture;

                Vector2 pos = mtile.transform.position;
                Vector2 esize = mtile.transform.size * mtile.transform.scale;
                float rotation = mtile.transform.rotation;

                vertex *v = &render_state->vertices[render_state->vertexCount];

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

                render_state->render_commands.push(draw_texture);
            }
        }
    }
}

void delete_scene(render_context *context)
{
    glDeleteVertexArrays(1, &context->VAO);
    glDeleteBuffers(1, &context->VBO);
    glDeleteBuffers(1, &context->EBO);
}

void check_compile_errors(unsigned int object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            printf("Shader: %s", infoLog);
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            printf("Program: %s", infoLog);
        }
    }
}

// TODO (Nico): Refactor file loading
std::string read_file(const std::string& filepath) {
		std::ifstream ifs((filepath).c_str());
		std::string content(
			std::istreambuf_iterator<char>(ifs.rdbuf()),
			std::istreambuf_iterator<char>()
			);
		return content;
	}

render_shader compile_shader(const char *vPath, const char *fPath)
{

    std::string vertexSourceS = read_file(vPath);
    std::string fragmentSourceS = read_file(fPath);
    const char *vertexSource = vertexSourceS.c_str();
    const char *fragmentSource = fragmentSourceS.c_str();

    render_shader shader;
    unsigned int sVertex, sFragment;
    // Vertex
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    check_compile_errors(sVertex, "VERTEX");

    // Frag
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    check_compile_errors(sFragment, "FRAGMENT");

    // Shader program
    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, sVertex);
    glAttachShader(shader.ID, sFragment);

    glLinkProgram(shader.ID);
    check_compile_errors(shader.ID, "PROGRAM");

    glDeleteShader(sVertex);
    glDeleteShader(sFragment);

    return shader;
}

Matrix create_projection(float zoom, Vector2 position, float rotation, float width, float height)
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

void create_framebuffer(render_framebuffer *fbo, const float width, const float height)
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
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is not complete");
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void bind_framebuffer(render_framebuffer *fbo, float width, float height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    glViewport(0, 0, width, height);
}
void unbind_framebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_framebuffer(render_framebuffer *fbo, float width, float height)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, fbo->width, fbo->height, 0, 0, width, height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
void delete_framebuffer(render_framebuffer *fbo)
{
    glDeleteFramebuffers(1, &fbo->fbo);
    glDeleteTextures(1, &fbo->texture);
    glDeleteRenderbuffers(1, &fbo->rbo);
}
