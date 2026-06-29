#pragma once

#include <glm/glm.hpp>
#include "game/BlockType.h"
#include "game/Resource.h"

struct PlayerStatus
{
    glm::vec3 position = glm::vec3(0.0f);
    bool grounded = false;
    bool flying = false;

    bool hasTarget = false;
    BlockType lookingAtBlock = BlockType::Air;

    Vitals vitals;
};
