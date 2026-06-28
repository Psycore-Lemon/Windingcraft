#pragma once

#include "graphics/Mesh.h"

class Chunk;

class ChunkMeshBuilder
{
public:
    static Mesh Build(const Chunk& chunk);
};
