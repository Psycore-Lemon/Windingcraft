#include "world/World.h"
#include "world/ChunkMeshBuilder.h"
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

    for (const auto& [key, data] : chunks)
    {
        std::string path = ChunkFilePath(key.x, key.y);
        ChunkSerializer::SaveToFile(*data.chunk, path);
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

    std::string path = ChunkFilePath(chunkX, chunkZ);
    bool loaded = !saveDir.empty() && ChunkSerializer::LoadFromFile(*chunk, path);

    if (!loaded)
        generator.Generate(*chunk);

    auto mesh = std::make_unique<Mesh>(ChunkMeshBuilder::Build(*chunk));

    glm::ivec2 key(chunkX, chunkZ);
    chunks[key] = ChunkData{ std::move(chunk), std::move(mesh) };
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

    return it->second.chunk->GetBlock(lx, b.y, lz);
}

bool World::SetBlock(const glm::vec3& worldPos, BlockType type)
{
    glm::ivec3 b = WorldToBlock(worldPos);

    int cx = (int)std::floor((float)b.x / Chunk::SIZE);
    int cz = (int)std::floor((float)b.z / Chunk::SIZE);

    auto it = chunks.find(glm::ivec2(cx, cz));
    if (it == chunks.end())
        return false;

    int lx = b.x - cx * Chunk::SIZE;
    int lz = b.z - cz * Chunk::SIZE;

    it->second.chunk->SetBlock(lx, b.y, lz, type);
    it->second.mesh = std::make_unique<Mesh>(ChunkMeshBuilder::Build(*it->second.chunk));

    return true;
}

glm::ivec2 World::WorldToChunk(const glm::vec3& position) const
{
    return glm::ivec2(
        (int)std::floor(position.x / Chunk::SIZE),
        (int)std::floor(position.z / Chunk::SIZE)
    );
}
