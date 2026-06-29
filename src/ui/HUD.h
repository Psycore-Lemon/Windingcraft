#pragma once

#include <glm/glm.hpp>

class HUD
{
public:
    void RenderOverlay(const glm::vec3& playerPos, bool grounded, bool flying,
                       int chunkCount, const char* lookingAt);
    void RenderCrosshair();
};
