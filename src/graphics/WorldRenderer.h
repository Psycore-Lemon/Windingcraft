#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

#include "graphics/Mesh.h"
#include "world/ChunkKey.h"

class Chunk;
class Camera;
class Shader;
class Renderer;
class World;

class WorldRenderer
{
public:
    void Sync(World& world);
    void RebuildChunkMesh(const glm::ivec2& key, const Chunk& chunk);
    void Render(const Renderer& renderer, const Shader& shader, const Camera& camera, float aspectRatio);
    int GetMeshCount() const;

private:
    std::unordered_map<glm::ivec2, std::unique_ptr<Mesh>, ChunkKeyHash> meshes;
};
