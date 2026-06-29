#include "graphics/WorldRenderer.h"
#include "graphics/Renderer.h"
#include "graphics/Shader.h"
#include "world/World.h"
#include "world/Chunk.h"
#include "world/ChunkMeshBuilder.h"

#include <glm/gtc/matrix_transform.hpp>

void WorldRenderer::Sync(World& world)
{
    for (const auto& [key, chunk] : world.GetChunks())
    {
        if (world.IsChunkDirty(key))
        {
            RebuildChunkMesh(key, *chunk);
            world.ClearChunkDirty(key);
        }
    }
}

void WorldRenderer::RebuildChunkMesh(const glm::ivec2& key, const Chunk& chunk)
{
    meshes[key] = std::make_unique<Mesh>(ChunkMeshBuilder::Build(chunk));
}

void WorldRenderer::Render(const Renderer& renderer, const Shader& shader,
                           const Camera& camera, float aspectRatio)
{
    glm::mat4 identity(1.0f);

    for (const auto& [key, mesh] : meshes)
    {
        if (mesh)
            renderer.Draw(*mesh, shader, camera, identity, aspectRatio);
    }
}

int WorldRenderer::GetMeshCount() const
{
    return (int)meshes.size();
}
