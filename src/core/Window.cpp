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

bool Window::Init(int width, int height, const std::string& title)
{
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!handle)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(handle);

    if (!gladLoadGL(glfwGetProcAddress))
        return false;

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

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
