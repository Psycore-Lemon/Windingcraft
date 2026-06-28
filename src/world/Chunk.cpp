#include "world/Chunk.h"

Chunk::Chunk(int chunkX, int chunkZ)
    : chunkX(chunkX), chunkZ(chunkZ)
{
}

void Chunk::SetBlock(int localX, int y, int localZ, bool solid)
{
    if (InBounds(localX, y, localZ))
        blocks[Index(localX, y, localZ)] = solid;
}

bool Chunk::GetBlock(int localX, int y, int localZ) const
{
    if (!InBounds(localX, y, localZ))
        return false;
    return blocks[Index(localX, y, localZ)];
}

glm::vec3 Chunk::BlockWorldPosition(int localX, int y, int localZ) const
{
    return glm::vec3(
        chunkX * SIZE + localX,
        y,
        chunkZ * SIZE + localZ
    );
}

std::vector<glm::vec3> Chunk::GetAllBlocks() const
{
    std::vector<glm::vec3> result;

    for (int x = 0; x < SIZE; ++x)
        for (int y = 0; y < MAX_HEIGHT; ++y)
            for (int z = 0; z < SIZE; ++z)
                if (blocks[Index(x, y, z)])
                    result.push_back(BlockWorldPosition(x, y, z));

    return result;
}

std::vector<AABB> Chunk::GetNearbyAABBs(const glm::vec3& position, float radius) const
{
    std::vector<AABB> result;
    glm::vec3 half(0.5f);

    int worldOffsetX = chunkX * SIZE;
    int worldOffsetZ = chunkZ * SIZE;

    int minX = std::max(0, (int)(position.x - radius) - worldOffsetX);
    int maxX = std::min(SIZE - 1, (int)(position.x + radius) - worldOffsetX);
    int minY = std::max(0, (int)(position.y - radius));
    int maxY = std::min(MAX_HEIGHT - 1, (int)(position.y + radius));
    int minZ = std::max(0, (int)(position.z - radius) - worldOffsetZ);
    int maxZ = std::min(SIZE - 1, (int)(position.z + radius) - worldOffsetZ);

    for (int x = minX; x <= maxX; ++x)
        for (int y = minY; y <= maxY; ++y)
            for (int z = minZ; z <= maxZ; ++z)
                if (blocks[Index(x, y, z)])
                    result.push_back(AABB::FromCenter(BlockWorldPosition(x, y, z), half));

    return result;
}

int Chunk::Index(int x, int y, int z) const
{
    return (y * SIZE * SIZE) + (z * SIZE) + x;
}

bool Chunk::InBounds(int x, int y, int z) const
{
    return x >= 0 && x < SIZE &&
           y >= 0 && y < MAX_HEIGHT &&
           z >= 0 && z < SIZE;
}
