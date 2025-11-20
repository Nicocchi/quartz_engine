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

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Window *win = (Window*)glfwGetWindowUserPointer(window);
    if (key <= 1024)
    {
        win->Input->keys[key] = action != GLFW_RELEASE;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    Window *win = (Window*)glfwGetWindowUserPointer(window);
    if (button <= 32)
    {
        win->Input->mouse_buttons[button] = action != GLFW_RELEASE;
    }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    Window *win = (Window*)glfwGetWindowUserPointer(window);
    win->Input->mx = xpos;
    win->Input->my = ypos;
}

bool create_window(Window *window, const char *title, int width, int height)
{

    window->title = title;
    window->width = width;
    window->height = height;
    window->closed = false;
    window->vwidth = 1280;
    window->vheight = 720;

    window->Input = new input_state();


    for (int i = 0; i < 1024; i++)
    {
        window->Input->keys[i] = false;
    }

    for (int i = 0; i < 32; i++)
    {
        window->Input->mouse_buttons[i] = false;
    }
    window->Input->mx = 0;
    window->Input->my = 0;

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
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    window->window = glfwCreateWindow(window->width, window->height, window->title, nullptr, nullptr);
    if (!window->window)
    {
        // QZ_ASSERT(false, "Could not create GLFW Window");
        return false;
    }

    // glfwSetWindowUserPointer(window->window, (void*)&window);
    glfwSetWindowUserPointer(window->window, window);
    glfwSetFramebufferSizeCallback(window->window, framebuffer_size_callback);
    glfwSetKeyCallback(window->window, key_callback);
    glfwSetMouseButtonCallback(window->window, mouse_button_callback);
    glfwSetCursorPosCallback(window->window, cursor_position_callback);
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
