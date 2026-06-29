#pragma once

#include <glm/glm.hpp>
#include <array>
#include <vector>

#include "game/BlockType.h"
#include "game/GameConfig.h"
#include "physics/AABB.h"

class Chunk
{
public:
    static constexpr int SIZE = GameConfig::ChunkSize;
    static constexpr int MAX_HEIGHT = GameConfig::ChunkMaxHeight;

    Chunk(int chunkX, int chunkZ);

    void SetBlock(int localX, int y, int localZ, BlockType type);
    BlockType GetBlock(int localX, int y, int localZ) const;

    bool IsSolid(int localX, int y, int localZ) const;

    glm::vec3 BlockWorldPosition(int localX, int y, int localZ) const;

    std::vector<AABB> GetNearbyAABBs(const glm::vec3& position, float radius) const;

    int GetChunkX() const { return chunkX; }
    int GetChunkZ() const { return chunkZ; }

private:
    int chunkX;
    int chunkZ;

    std::array<BlockType, SIZE * MAX_HEIGHT * SIZE> blocks = {};

    int Index(int x, int y, int z) const;
    bool InBounds(int x, int y, int z) const;
};
