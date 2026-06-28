#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "physics/AABB.h"

class World
{
public:
    void AddBlock(const glm::vec3& position);

    const std::vector<glm::vec3>& GetBlocks() const;

    std::vector<AABB> GetNearbyAABBs(const glm::vec3& position, float radius) const;

private:
    std::vector<glm::vec3> blocks;
};
