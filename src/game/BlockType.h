#pragma once

#include <cstdint>

enum class BlockType : uint16_t
{
    Air = 0,
    Stone,
    Dirt,

    Count
};

struct BlockDef
{
    const char* name;
    bool solid;
    float r, g, b;
};

namespace Blocks
{
    constexpr BlockDef definitions[] = {
        { "Air",   false, 0.0f, 0.0f, 0.0f },
        { "Stone", true,  0.5f, 0.5f, 0.5f },
        { "Dirt",  true,  0.45f, 0.3f, 0.15f },
    };

    constexpr const BlockDef& Get(BlockType type)
    {
        return definitions[static_cast<uint16_t>(type)];
    }

    constexpr bool IsSolid(BlockType type)
    {
        return definitions[static_cast<uint16_t>(type)].solid;
    }
}
