#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include "physics/AABB.h"
#include "graphics/Mesh.h"
#include "world/Chunk.h"
#include "world/TerrainGenerator.h"

struct ChunkKeyHash
{
    std::size_t operator()(const glm::ivec2& k) const
    {
        return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 16);
    }
};

struct ChunkData
{
    std::unique_ptr<Chunk> chunk;
    std::unique_ptr<Mesh> mesh;
};

class World
{
public:
    World(int seed = 1337, int loadRadius = 8);

    void SetSaveDir(const std::string& dir);

    void Update(const glm::vec3& playerPosition);
    void SaveAll();

    BlockType GetBlock(const glm::vec3& worldPos) const;
    bool SetBlock(const glm::vec3& worldPos, BlockType type);

    std::vector<AABB> GetNearbyAABBs(const glm::vec3& position, float radius) const;

    const std::unordered_map<glm::ivec2, ChunkData, ChunkKeyHash>& GetChunks() const;

private:
    void LoadChunk(int chunkX, int chunkZ);
    std::string ChunkFilePath(int chunkX, int chunkZ) const;

    glm::ivec2 WorldToChunk(const glm::vec3& position) const;

    TerrainGenerator generator;
    int loadRadius;
    std::string saveDir;

    std::unordered_map<glm::ivec2, ChunkData, ChunkKeyHash> chunks;
};
