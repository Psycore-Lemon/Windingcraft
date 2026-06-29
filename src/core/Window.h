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

    bool Init(int width, int height, const std::string& title, bool fullscreen = false);
    bool ShouldClose() const;
    void Close();

    GLFWwindow* GetHandle() const;
    void SetUserPointer(void* ptr);
    float GetAspectRatio() const;
    void SetCursorCaptured(bool captured);
    void SetSize(int width, int height);
    void SetFullscreen(bool fullscreen);
    bool IsFullscreen() const;

private:
    GLFWwindow* handle = nullptr;
    int windowedWidth = 1280;
    int windowedHeight = 720;
};
