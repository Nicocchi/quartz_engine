#include "window.hpp"

Window::Window()
{
    width = 1366;
    height = 768;
    title = "Quartz Engine";

    for (size_t i = 0; i < 1024; i++)
    {
        keys[i] = 0;
    }

    xChange = 0.0f;
    yChange = 0.0f;
}

Window::Window(GLint w, GLint h, const char* t) : width(w), height(h), title(t)
{
    for (size_t i = 0; i < 1024; i++)
    {
        keys[i] = 0;
    }

    xChange = 0.0f;
    yChange = 0.0f;
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::init()
{
    // Init GLFW
    if (!glfwInit())
    {
        printf("GLFW Initialization failed");
        glfwTerminate();
        return false;
    }

    // GLFW Window properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window)
    {
        printf("GLFW window creation failed");
        glfwTerminate();
        return false;
    }

    // Get buffer size info
    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

    // Set context for GLEW
    glfwMakeContextCurrent(window);

    // Handle Key + Mouse Input
    createCallbacks();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Allow modern extensions
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW Initialization failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    glfwSetWindowUserPointer(window, this);

    glfwSwapInterval(0);

    return true;
}

void Window::createCallbacks()
{
    glfwSetKeyCallback(window, handleKeys);
    glfwSetCursorPosCallback(window, handleMouse);
}

GLfloat Window::getXChange()
{
    GLfloat change = xChange;
    xChange = 0.0f;
    return change;
}

GLfloat Window::getYChange()
{
    GLfloat change = yChange;
    yChange = 0.0f;
    return change;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            win->keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            win->keys[key] = false;
        }
    }
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (win->mouseFirstMoved)
    {
        win->lastX = xPos;
        win->lastY = yPos;
        win->mouseFirstMoved = false;
    }

    win->xChange = xPos - win->lastX;
    win->yChange = win->lastY - yPos;
    // win->yChange = yPos - win->lastY; // Inverted
    win->lastX = xPos;
    win->lastY = yPos;
}