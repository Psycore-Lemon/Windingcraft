#pragma once

#include <glm/glm.hpp>
#include <algorithm>

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;

    static AABB FromCenter(const glm::vec3& center, const glm::vec3& halfExtents)
    {
        return { center - halfExtents, center + halfExtents };
    }

    bool Overlaps(const AABB& other) const
    {
        return min.x < other.max.x && max.x > other.min.x &&
               min.y < other.max.y && max.y > other.min.y &&
               min.z < other.max.z && max.z > other.min.z;
    }

    // Returns the axis and sign of minimum penetration.
    // The result is the vector to push *this* out of other.
    glm::vec3 GetPenetration(const AABB& other) const
    {
        float dx1 = other.max.x - min.x;
        float dx2 = max.x - other.min.x;
        float dy1 = other.max.y - min.y;
        float dy2 = max.y - other.min.y;
        float dz1 = other.max.z - min.z;
        float dz2 = max.z - other.min.z;

        float overlapX = (dx1 < dx2) ? dx1 : -dx2;
        float overlapY = (dy1 < dy2) ? dy1 : -dy2;
        float overlapZ = (dz1 < dz2) ? dz1 : -dz2;

        float absX = std::abs(overlapX);
        float absY = std::abs(overlapY);
        float absZ = std::abs(overlapZ);

        if (absX <= absY && absX <= absZ)
            return { overlapX, 0.0f, 0.0f };
        if (absY <= absX && absY <= absZ)
            return { 0.0f, overlapY, 0.0f };
        return { 0.0f, 0.0f, overlapZ };
    }
};
