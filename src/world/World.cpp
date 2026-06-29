#include "world/World.h"
#include "world/ChunkSerializer.h"

#include <cmath>
#include <filesystem>

namespace fs = std::filesystem;

World::World(int seed, int loadRadius)
    : generator(seed), loadRadius(loadRadius)
{
}

void World::SetSaveDir(const std::string& dir)
{
    saveDir = dir;
    fs::create_directories(fs::path(dir) / "chunks");
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

void World::SaveAll()
{
    if (saveDir.empty())
        return;

    for (const auto& [key, chunk] : chunks)
    {
        std::string path = ChunkFilePath(key.x, key.y);
        ChunkSerializer::SaveToFile(*chunk, path);
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

            auto chunkAABBs = it->second->GetNearbyAABBs(position, radius);
            result.insert(result.end(), chunkAABBs.begin(), chunkAABBs.end());
        }
    }

    return result;
}

const std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, ChunkKeyHash>& World::GetChunks() const
{
    return chunks;
}

bool World::IsChunkDirty(const glm::ivec2& key) const
{
    auto it = dirtyChunks.find(key);
    return it != dirtyChunks.end() && it->second;
}

void World::ClearChunkDirty(const glm::ivec2& key)
{
    dirtyChunks[key] = false;
}

void World::LoadChunk(int chunkX, int chunkZ)
{
    auto chunk = std::make_unique<Chunk>(chunkX, chunkZ);

    std::string path = ChunkFilePath(chunkX, chunkZ);
    bool loaded = !saveDir.empty() && ChunkSerializer::LoadFromFile(*chunk, path);

    if (!loaded)
        generator.Generate(*chunk);

    glm::ivec2 key(chunkX, chunkZ);
    chunks[key] = std::move(chunk);
    dirtyChunks[key] = true;
}

std::string World::ChunkFilePath(int chunkX, int chunkZ) const
{
    return (fs::path(saveDir) / "chunks" /
            (std::to_string(chunkX) + "_" + std::to_string(chunkZ) + ".chunk")).string();
}

static glm::ivec3 WorldToBlock(const glm::vec3& pos)
{
    return glm::ivec3(
        (int)std::floor(pos.x),
        (int)std::floor(pos.y),
        (int)std::floor(pos.z)
    );
}

BlockType World::GetBlock(const glm::vec3& worldPos) const
{
    glm::ivec3 b = WorldToBlock(worldPos);

    int cx = (int)std::floor((float)b.x / Chunk::SIZE);
    int cz = (int)std::floor((float)b.z / Chunk::SIZE);

    auto it = chunks.find(glm::ivec2(cx, cz));
    if (it == chunks.end())
        return BlockType::Air;

    int lx = b.x - cx * Chunk::SIZE;
    int lz = b.z - cz * Chunk::SIZE;

    return it->second->GetBlock(lx, b.y, lz);
}

bool World::SetBlock(const glm::vec3& worldPos, BlockType type)
{
    glm::ivec3 b = WorldToBlock(worldPos);

    int cx = (int)std::floor((float)b.x / Chunk::SIZE);
    int cz = (int)std::floor((float)b.z / Chunk::SIZE);

    glm::ivec2 key(cx, cz);
    auto it = chunks.find(key);
    if (it == chunks.end())
        return false;

    int lx = b.x - cx * Chunk::SIZE;
    int lz = b.z - cz * Chunk::SIZE;

    it->second->SetBlock(lx, b.y, lz, type);
    dirtyChunks[key] = true;

    return true;
}

glm::ivec2 World::WorldToChunk(const glm::vec3& position) const
{
    return glm::ivec2(
        (int)std::floor(position.x / Chunk::SIZE),
        (int)std::floor(position.z / Chunk::SIZE)
    );
}
