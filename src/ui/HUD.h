#pragma once

#include <glm/glm.hpp>

struct PlayerStatus;

class HUD
{
public:
    void RenderOverlay(const PlayerStatus& status, int chunkCount);
    void RenderCrosshair();
    void RenderResourceBars(const PlayerStatus& status);
};
