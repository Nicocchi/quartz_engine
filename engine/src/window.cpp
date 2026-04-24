#include "window.hpp"

void debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
        GLsizei length, const GLchar *message, const void* userParam)
{
    // https://learnopengl.com/In-Practice/Debugging
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    EngineLogger *engineLogger = static_cast<EngineLogger*>(const_cast<void*>(userParam));

    printf("---------------\n");
    // std::string log = "Debug message" + static_cast<int>(id) + ": " + message;
    std::string msg = message;
    std::string msg_id = std::to_string(id);
    std::string log = "Debug message (" + msg_id + "): " + msg;
    printf("Debug message (%i): %s\n", id, message);

    EngineLog elog;
    elog.type = LogType::SYSTEM;
    elog.messages.push_back(log);

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             printf("Source: API"); if (engineLogger) elog.messages.push_back("Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   printf("Source: Window System"); if (engineLogger) elog.messages.push_back("Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: printf("Source: Shader Compiler"); if (engineLogger) elog.messages.push_back("Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     printf("Source: Third Party"); if (engineLogger) elog.messages.push_back("Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:     printf("Source: Application"); if (engineLogger) elog.messages.push_back("Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:           printf("Source: Other"); if (engineLogger) elog.messages.push_back("Source: Other"); break;
    }
    printf("\n");

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               printf("Type: Error"); if (engineLogger) elog.messages.push_back("Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Type: Deprecated Behaviour"); if (engineLogger) elog.messages.push_back("Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  printf("Type: Undefined Behaviour"); if (engineLogger) elog.messages.push_back("Type: Undefined Behaviour"); break; 
        case GL_DEBUG_TYPE_PORTABILITY:         printf("Type: Portability"); if (engineLogger) elog.messages.push_back("Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         printf("Type: Performance"); if (engineLogger) elog.messages.push_back("Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:              printf("Type: Marker"); if (engineLogger) elog.messages.push_back("Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          printf("Type: Push Group"); if (engineLogger) elog.messages.push_back("Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           printf("Type: Pop Group"); if (engineLogger) elog.messages.push_back("Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:               printf("Type: Other"); if (engineLogger) elog.messages.push_back("Type: Other"); break;
    }
    printf("\n");
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         printf("Severity: high"); if (engineLogger) elog.messages.push_back("Severity: high"); elog.severity = LogSeverity::HIGH; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       printf("Severity: medium"); if (engineLogger) elog.messages.push_back("Severity: medium"); elog.severity = LogSeverity::MEDIUM; break;
        case GL_DEBUG_SEVERITY_LOW:          printf("Severity: low"); if (engineLogger) elog.messages.push_back("Severity: low"); elog.severity = LogSeverity::LOW; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: printf("Severity: notification"); if (engineLogger) elog.messages.push_back("Severity: notification"); elog.severity = LogSeverity::LOW; break;
    }
    printf("\n\n");

    engineLogger->logs.push_back(elog);
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
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            win->Input->keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            win->Input->keys[key] = false;
            win->Input->keys_processed[key] = false;
        }
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

bool create_window(Window *window, const char *title, int width, int height, EngineLogger *engineLog)
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
    window->width = mode->width;
    window->height = mode->height;

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
        return false;
    }

    int flags;
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_message_callback, engineLog);
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
