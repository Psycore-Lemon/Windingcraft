#include "world/ChunkSerializer.h"
#include "world/Chunk.h"

#include <fstream>
#include <unordered_map>
#include <cmath>

static int BitsNeeded(int paletteSize)
{
    if (paletteSize <= 1) return 1;
    return (int)std::ceil(std::log2(paletteSize));
}

static void IndexToXYZ(int i, int& x, int& y, int& z)
{
    y = i / (Chunk::SIZE * Chunk::SIZE);
    int rem = i % (Chunk::SIZE * Chunk::SIZE);
    z = rem / Chunk::SIZE;
    x = rem % Chunk::SIZE;
}

std::vector<uint8_t> ChunkSerializer::Serialize(const Chunk& chunk)
{
    constexpr int totalBlocks = Chunk::SIZE * Chunk::MAX_HEIGHT * Chunk::SIZE;

    std::vector<uint16_t> palette;
    std::unordered_map<uint16_t, uint16_t> typeToIndex;
    std::vector<uint16_t> blockIndices(totalBlocks);

    for (int i = 0; i < totalBlocks; ++i)
    {
        int x, y, z;
        IndexToXYZ(i, x, y, z);

        uint16_t type = static_cast<uint16_t>(chunk.GetBlock(x, y, z));

        auto it = typeToIndex.find(type);
        if (it == typeToIndex.end())
        {
            uint16_t idx = (uint16_t)palette.size();
            typeToIndex[type] = idx;
            palette.push_back(type);
            blockIndices[i] = idx;
        }
        else
        {
            blockIndices[i] = it->second;
        }
    }

    int bits = BitsNeeded((int)palette.size());
    int dataBytes = (totalBlocks * bits + 7) / 8;

    std::vector<uint8_t> result;
    result.reserve(2 + palette.size() * 2 + 1 + dataBytes);

    uint16_t paletteSize = (uint16_t)palette.size();
    result.push_back(paletteSize & 0xFF);
    result.push_back((paletteSize >> 8) & 0xFF);

    for (uint16_t entry : palette)
    {
        result.push_back(entry & 0xFF);
        result.push_back((entry >> 8) & 0xFF);
    }

    result.push_back((uint8_t)bits);

    std::vector<uint8_t> packed(dataBytes, 0);
    int bitPos = 0;

    for (int i = 0; i < totalBlocks; ++i)
    {
        uint16_t val = blockIndices[i];
        for (int b = 0; b < bits; ++b)
        {
            if (val & (1 << b))
                packed[bitPos / 8] |= (1 << (bitPos % 8));
            bitPos++;
        }
    }

    result.insert(result.end(), packed.begin(), packed.end());
    return result;
}

void ChunkSerializer::Deserialize(Chunk& chunk, const std::vector<uint8_t>& data)
{
    if (data.size() < 3)
        return;

    int pos = 0;

    uint16_t paletteSize = data[pos] | (data[pos + 1] << 8);
    pos += 2;

    std::vector<uint16_t> palette(paletteSize);
    for (int i = 0; i < paletteSize; ++i)
    {
        palette[i] = data[pos] | (data[pos + 1] << 8);
        pos += 2;
    }

    int bits = data[pos++];

    constexpr int totalBlocks = Chunk::SIZE * Chunk::MAX_HEIGHT * Chunk::SIZE;
    int bitPos = 0;

    for (int i = 0; i < totalBlocks; ++i)
    {
        uint16_t val = 0;
        for (int b = 0; b < bits; ++b)
        {
            int byteIdx = pos + (bitPos / 8);
            int bitIdx = bitPos % 8;

            if (byteIdx < (int)data.size() && (data[byteIdx] & (1 << bitIdx)))
                val |= (1 << b);

            bitPos++;
        }

        int x, y, z;
        IndexToXYZ(i, x, y, z);

        uint16_t blockType = (val < paletteSize) ? palette[val] : 0;
        chunk.SetBlock(x, y, z, static_cast<BlockType>(blockType));
    }
}

void ChunkSerializer::SaveToFile(const Chunk& chunk, const std::string& path)
{
    auto data = Serialize(chunk);
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

bool ChunkSerializer::LoadFromFile(Chunk& chunk, const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return false;

    auto size = file.tellg();
    file.seekg(0);

    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);

    Deserialize(chunk, data);
    return true;
}
