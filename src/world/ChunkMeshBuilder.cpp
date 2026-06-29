#include "world/ChunkMeshBuilder.h"
#include "world/Chunk.h"

#include <vector>

struct FaceData
{
    float vertices[4][5]; // 4 verts, each: x y z u v
    int normal[3];
};

static const FaceData faces[6] = {
    // +X (right)
    {{ {1,0,0, 0,0}, {1,1,0, 1,0}, {1,1,1, 1,1}, {1,0,1, 0,1} }, {1,0,0}},
    // -X (left)
    {{ {0,0,1, 0,0}, {0,1,1, 1,0}, {0,1,0, 1,1}, {0,0,0, 0,1} }, {-1,0,0}},
    // +Y (top)
    {{ {0,1,0, 0,0}, {1,1,0, 1,0}, {1,1,1, 1,1}, {0,1,1, 0,1} }, {0,1,0}},
    // -Y (bottom)
    {{ {0,0,1, 0,0}, {1,0,1, 1,0}, {1,0,0, 1,1}, {0,0,0, 0,1} }, {0,-1,0}},
    // +Z (front)
    {{ {0,0,1, 0,0}, {1,0,1, 1,0}, {1,1,1, 1,1}, {0,1,1, 0,1} }, {0,0,1}},
    // -Z (back)
    {{ {1,0,0, 0,0}, {0,0,0, 1,0}, {0,1,0, 1,1}, {1,1,0, 0,1} }, {0,0,-1}},
};

Mesh ChunkMeshBuilder::Build(const Chunk& chunk)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    unsigned int indexOffset = 0;

    for (int x = 0; x < Chunk::SIZE; ++x)
    {
        for (int y = 0; y < Chunk::MAX_HEIGHT; ++y)
        {
            for (int z = 0; z < Chunk::SIZE; ++z)
            {
                BlockType type = chunk.GetBlock(x, y, z);
                if (!Blocks::IsSolid(type))
                    continue;

                const BlockDef& def = Blocks::Get(type);
                float layer = static_cast<float>(def.textureLayer);
                glm::vec3 worldPos = chunk.BlockWorldPosition(x, y, z);

                for (int f = 0; f < 6; ++f)
                {
                    const auto& face = faces[f];
                    int nx = x + face.normal[0];
                    int ny = y + face.normal[1];
                    int nz = z + face.normal[2];

                    if (chunk.IsSolid(nx, ny, nz))
                        continue;

                    for (int v = 0; v < 4; ++v)
                    {
                        vertices.push_back(face.vertices[v][0] + worldPos.x);
                        vertices.push_back(face.vertices[v][1] + worldPos.y);
                        vertices.push_back(face.vertices[v][2] + worldPos.z);
                        vertices.push_back(face.vertices[v][3]);
                        vertices.push_back(face.vertices[v][4]);
                        vertices.push_back(layer);
                    }

                    indices.push_back(indexOffset + 0);
                    indices.push_back(indexOffset + 1);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 2);
                    indices.push_back(indexOffset + 3);
                    indices.push_back(indexOffset + 0);

                    indexOffset += 4;
                }
            }
        }
    }

    if (indices.empty())
    {
        return Mesh({0,0,0,0,0,0}, {0,0,0}, 6);
    }

    return Mesh(vertices, indices, 6);
}
