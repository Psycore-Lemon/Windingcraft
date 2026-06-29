#include "core/Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

Window::~Window()
{
    if (handle)
    {
        glfwDestroyWindow(handle);
        handle = nullptr;
    }
    glfwTerminate();
}

bool Window::Init(int width, int height, const std::string& title, bool fullscreen)
{
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    windowedWidth = width;
    windowedHeight = height;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (fullscreen)
    {
        handle = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
    }
    else
    {
        handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    }

    if (!handle)
    {
        glfwTerminate();
        return false;
    }

    if (!fullscreen)
    {
        int x = (mode->width - width) / 2;
        int y = (mode->height - height) / 2;
        glfwSetWindowPos(handle, x, y);
    }

    glfwMakeContextCurrent(handle);

    if (!gladLoadGL(glfwGetProcAddress))
        return false;

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(handle, [](GLFWwindow*, int w, int h) {
        glViewport(0, 0, w, h);
    });

    return true;
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(handle);
}

void Window::Close()
{
    glfwSetWindowShouldClose(handle, true);
}

GLFWwindow* Window::GetHandle() const
{
    return handle;
}

void Window::SetUserPointer(void* ptr)
{
    glfwSetWindowUserPointer(handle, ptr);
}

float Window::GetAspectRatio() const
{
    int w, h;
    glfwGetFramebufferSize(handle, &w, &h);
    if (h == 0) return 1.0f;
    return static_cast<float>(w) / static_cast<float>(h);
}

void Window::SetCursorCaptured(bool captured)
{
    glfwSetInputMode(handle, GLFW_CURSOR,
        captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::SetSize(int width, int height)
{
    if (IsFullscreen())
        return;

    windowedWidth = width;
    windowedHeight = height;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int x = (mode->width - width) / 2;
    int y = (mode->height - height) / 2;

    glfwSetWindowSize(handle, width, height);
    glfwSetWindowPos(handle, x, y);
}

void Window::SetFullscreen(bool fullscreen)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (fullscreen)
    {
        glfwSetWindowMonitor(handle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        int x = (mode->width - windowedWidth) / 2;
        int y = (mode->height - windowedHeight) / 2;
        glfwSetWindowMonitor(handle, nullptr, x, y, windowedWidth, windowedHeight, 0);
    }
}

bool Window::IsFullscreen() const
{
    return glfwGetWindowMonitor(handle) != nullptr;
}
