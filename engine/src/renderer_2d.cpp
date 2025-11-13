void init_renderer(render_context *context, const float width, const float height)
{

    context->shader = compile_shader("assets/shaders/basic.vs", "assets/shaders/basic.frag");

    // GLfloat vertices[] = {
    //     -1.0f, -1.0f, 0.0f,  // Bottom-Left
    //     -1.0f,  1.0f, 0.0f,  // Top-Left
    //     1.0f,  1.0f, 0.0f,  // Top-Right
    //     1.0f, -1.0f, 0.0f   // Bottom-Right
    // };

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
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(pos_index, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(0);

    
    // UVs
    // glBindBuffer(GL_ARRAY_BUFFER, context->UVO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(uv_data), uv_data, GL_STATIC_DRAW);
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
                // glBindTexture(GL_TEXTURE_2D, context->defaultWhiteTexture);

                glDrawElementsBaseVertex(GL_TRIANGLES, command.count, GL_UNSIGNED_INT, 0, command.offset);
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);

            } break;

            case COMPILE_SHADER:
            {
                context->shader = compile_shader(command.shader->vertexPath, command.shader->fragmentPath);
            } break;

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
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
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
void delete_framebuffer(render_framebuffer *fbo)
{
    glDeleteFramebuffers(1, &fbo->fbo);
    glDeleteTextures(1, &fbo->texture);
    glDeleteRenderbuffers(1, &fbo->rbo);
}
