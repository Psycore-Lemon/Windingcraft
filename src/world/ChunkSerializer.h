#pragma once

#include <string>
#include <vector>
#include <cstdint>

class Chunk;

class ChunkSerializer
{
public:
    static std::vector<uint8_t> Serialize(const Chunk& chunk);
    static void Deserialize(Chunk& chunk, const std::vector<uint8_t>& data);

    static void SaveToFile(const Chunk& chunk, const std::string& path);
    static bool LoadFromFile(Chunk& chunk, const std::string& path);
};
