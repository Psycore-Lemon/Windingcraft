#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include "physics/AABB.h"
#include "world/Chunk.h"
#include "world/ChunkKey.h"
#include "world/TerrainGenerator.h"

class World
{
public:
    World(int seed, int loadRadius);

    void SetSaveDir(const std::string& dir);

    void Update(const glm::vec3& playerPosition);
    void SaveAll();

    BlockType GetBlock(const glm::vec3& worldPos) const;
    bool SetBlock(const glm::vec3& worldPos, BlockType type);

    std::vector<AABB> GetNearbyAABBs(const glm::vec3& position, float radius) const;

    const std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, ChunkKeyHash>& GetChunks() const;

    bool IsChunkDirty(const glm::ivec2& key) const;
    void ClearChunkDirty(const glm::ivec2& key);

private:
    void LoadChunk(int chunkX, int chunkZ);
    std::string ChunkFilePath(int chunkX, int chunkZ) const;

    glm::ivec2 WorldToChunk(const glm::vec3& position) const;

    TerrainGenerator generator;
    int loadRadius;
    std::string saveDir;

    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, ChunkKeyHash> chunks;
    std::unordered_map<glm::ivec2, bool, ChunkKeyHash> dirtyChunks;
};
