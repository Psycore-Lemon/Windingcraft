#pragma once

#include "game/GameConfig.h"

class Chunk;

class TerrainGenerator
{
public:
    TerrainGenerator(int seed = GameConfig::DefaultSeed);

    void Generate(Chunk& chunk) const;

private:
    int seed;
    int baseHeight = GameConfig::TerrainBaseHeight;
    int heightRange = GameConfig::TerrainHeightRange;
};
