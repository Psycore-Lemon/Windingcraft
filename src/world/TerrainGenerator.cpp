#include "world/TerrainGenerator.h"
#include "world/Chunk.h"

#include <FastNoiseLite.h>

TerrainGenerator::TerrainGenerator(int seed)
    : seed(seed)
{
}

void TerrainGenerator::Generate(Chunk& chunk) const
{
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.02f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(4);

    for (int x = 0; x < Chunk::SIZE; ++x)
    {
        for (int z = 0; z < Chunk::SIZE; ++z)
        {
            float worldX = (float)(chunk.GetChunkX() * Chunk::SIZE + x);
            float worldZ = (float)(chunk.GetChunkZ() * Chunk::SIZE + z);

            float n = noise.GetNoise(worldX, worldZ);

            int height = baseHeight + (int)(n * heightRange);

            for (int y = 0; y <= height; ++y)
                chunk.SetBlock(x, y, z, true);
        }
    }
}
