#pragma once

class Chunk;

class TerrainGenerator
{
public:
    TerrainGenerator(int seed = 1337);

    void Generate(Chunk& chunk) const;

private:
    int seed;
    int baseHeight = 10;
    int heightRange = 8;
};
