#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool Init(int width, int height, const std::string& title);
    bool ShouldClose() const;
    void Close();

    GLFWwindow* GetHandle() const;

private:
    GLFWwindow* handle = nullptr;
};
