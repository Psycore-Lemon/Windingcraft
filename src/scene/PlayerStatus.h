#pragma once

#include <glm/glm.hpp>
#include "game/BlockType.h"

struct PlayerStatus
{
    glm::vec3 position = glm::vec3(0.0f);
    bool grounded = false;
    bool flying = false;

    bool hasTarget = false;
    BlockType lookingAtBlock = BlockType::Air;
};
