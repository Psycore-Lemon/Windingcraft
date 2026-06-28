#include "scene/World.h"

void World::AddBlock(const glm::vec3& position)
{
    blocks.push_back(position);
}

const std::vector<glm::vec3>& World::GetBlocks() const
{
    return blocks;
}

std::vector<AABB> World::GetNearbyAABBs(const glm::vec3& position, float radius) const
{
    std::vector<AABB> result;
    glm::vec3 half(0.5f);

    for (const auto& block : blocks)
    {
        if (glm::length(block - position) < radius)
        {
            result.push_back(AABB::FromCenter(block, half));
        }
    }

    return result;
}
