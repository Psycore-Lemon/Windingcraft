#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

class HUD
{
public:
    HUD() = default;
    ~HUD();

    HUD(const HUD&) = delete;
    HUD& operator=(const HUD&) = delete;

    void Init(GLFWwindow* window);
    void BeginFrame(bool interactive);
    void RenderOverlay(const glm::vec3& playerPos, bool grounded, int chunkCount);
    void RenderCrosshair();

    enum class PauseAction { None, Resume, Quit };
    PauseAction RenderPauseMenu();

    void EndFrame();

private:
    GLFWwindow* window = nullptr;
    bool initialized = false;
};
