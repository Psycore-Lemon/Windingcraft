#pragma once

struct GLFWwindow;

class UIManager
{
public:
    UIManager() = default;
    ~UIManager();

    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    void Init(GLFWwindow* window);
    void BeginFrame(bool interactive);
    void EndFrame();

private:
    GLFWwindow* window = nullptr;
    bool initialized = false;
};
