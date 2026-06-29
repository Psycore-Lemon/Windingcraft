#pragma once

#include <glm/glm.hpp>
#include "game/BlockType.h"

struct PlayerCommand
{
    glm::vec3 moveDirection{0.0f};
    glm::vec3 lookDirection{0.0f, 0.0f, -1.0f};

    bool jump = false;
    bool descend = false;
    bool primaryAction = false;
    bool secondaryAction = false;

    float scrollDelta = 0.0f;
    int hotbarSelect = -1;
};
