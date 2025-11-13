void debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar *message, const void* userParam)
{
    // printf("OpenGL: %s\n", message);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Window *win =(Window*)glfwGetWindowUserPointer(window);
    win->width = width;
    win->height = height;
}

bool create_window(Window *window, const char *title, int width, int height)
{
    
    window->title = title;
    window->width = width;
    window->height = height;
    window->closed = false;
    window->vwidth = 1280;
    window->vheight = 720;

    if (!glfwInit())
    {
        // QZ_ASSERT(false, "Could not initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    window->width = mode->width;
    window->height = mode->height;
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    window->window = glfwCreateWindow(window->width, window->height, window->title, nullptr, nullptr);
    if (!window->window)
    {
        // QZ_ASSERT(false, "Could not create GLFW Window");
        return false;
    }

    glfwSetWindowUserPointer(window->window, (void*)&window);
    glfwSetFramebufferSizeCallback(window->window, framebuffer_size_callback);

    glfwMakeContextCurrent(window->window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        // QZ_ASSERT(false, "Could not initialize GLAD");
        return -1;
    }

    int flags;
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_message_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glViewport(0, 0, window->vwidth, window->vheight);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // QZ_LOG("OpenGL: %s", glGetString(GL_VERSION));

    return true;
}

bool window_closed(Window *window)
{
    return glfwWindowShouldClose(window->window) == 1;
}

void window_poll()
{
    glfwPollEvents();
}

void window_swap_buffers(Window *window)
{
    glfwSwapBuffers(window->window);
}
void window_clear(Vector3 color)
{
    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}