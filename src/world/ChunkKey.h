#pragma once

#include <glm/glm.hpp>
#include <functional>

struct ChunkKeyHash
{
    std::size_t operator()(const glm::ivec2& k) const
    {
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 16);
    }
};
