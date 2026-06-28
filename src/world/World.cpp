#include "world/World.h"
#include "world/ChunkMeshBuilder.h"

#include <cmath>

World::World(int seed, int loadRadius)
    : generator(seed), loadRadius(loadRadius)
{
}

void World::Update(const glm::vec3& playerPosition)
{
    glm::ivec2 center = WorldToChunk(playerPosition);

    for (int dx = -loadRadius; dx <= loadRadius; ++dx)
    {
        for (int dz = -loadRadius; dz <= loadRadius; ++dz)
        {
            glm::ivec2 key(center.x + dx, center.y + dz);

            if (chunks.find(key) == chunks.end())
                LoadChunk(key.x, key.y);
        }
    }
}

std::vector<AABB> World::GetNearbyAABBs(const glm::vec3& position, float radius) const
{
    std::vector<AABB> result;

    int minCX = (int)std::floor((position.x - radius) / Chunk::SIZE);
    int maxCX = (int)std::floor((position.x + radius) / Chunk::SIZE);
    int minCZ = (int)std::floor((position.z - radius) / Chunk::SIZE);
    int maxCZ = (int)std::floor((position.z + radius) / Chunk::SIZE);

    for (int cx = minCX; cx <= maxCX; ++cx)
    {
        for (int cz = minCZ; cz <= maxCZ; ++cz)
        {
            auto it = chunks.find(glm::ivec2(cx, cz));
            if (it == chunks.end())
                continue;

            auto chunkAABBs = it->second.chunk->GetNearbyAABBs(position, radius);
            result.insert(result.end(), chunkAABBs.begin(), chunkAABBs.end());
        }
    }

    return result;
}

const std::unordered_map<glm::ivec2, ChunkData, ChunkKeyHash>& World::GetChunks() const
{
    return chunks;
}

void World::LoadChunk(int chunkX, int chunkZ)
{
    auto chunk = std::make_unique<Chunk>(chunkX, chunkZ);
    generator.Generate(*chunk);

    auto mesh = std::make_unique<Mesh>(ChunkMeshBuilder::Build(*chunk));

    glm::ivec2 key(chunkX, chunkZ);
    chunks[key] = ChunkData{ std::move(chunk), std::move(mesh) };
}

glm::ivec2 World::WorldToChunk(const glm::vec3& position) const
{
    return glm::ivec2(
        (int)std::floor(position.x / Chunk::SIZE),
        (int)std::floor(position.z / Chunk::SIZE)
    );
}
