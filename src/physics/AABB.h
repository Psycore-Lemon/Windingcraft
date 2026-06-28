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

    glm::vec3 GetPenetrationAxis(const AABB& other, int axis) const
    {
        float overlaps[3];
        for (int i = 0; i < 3; ++i)
        {
            float d1 = other.max[i] - min[i];
            float d2 = max[i] - other.min[i];
            overlaps[i] = std::min(d1, d2);
        }

        if (overlaps[axis] > overlaps[(axis + 1) % 3] ||
            overlaps[axis] > overlaps[(axis + 2) % 3])
            return glm::vec3(0.0f);

        float d1 = other.max[axis] - min[axis];
        float d2 = max[axis] - other.min[axis];
        float push = (d1 < d2) ? d1 : -d2;

        glm::vec3 result(0.0f);
        result[axis] = push;
        return result;
    }
};
