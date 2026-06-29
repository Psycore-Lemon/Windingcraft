#pragma once

#include "net/ByteBuffer.h"
#include "net/NetworkTypes.h"
#include "game/PlayerCommand.h"
#include "game/BlockType.h"
#include "game/Inventory.h"
#include "game/Resource.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct PlayerSnapshot
{
    int playerId = -1;
    std::string username;  // populated client-side from cache, not sent per-tick
    glm::vec3 position{0.0f};
    bool grounded = false;
    bool flying = false;
    Vitals vitals;
    bool hasTarget = false;
    BlockType lookingAtBlock = BlockType::Air;
    glm::ivec3 lookingAtBlockPos{0};

    int selectedIndex = 0;
    InventorySlot hotbar[Inventory::HOTBAR_SIZE] = {};
};

struct BlockChange
{
    glm::ivec3 position{0};
    BlockType type = BlockType::Air;
};

struct ChunkPacket
{
    glm::ivec2 key{0};
    std::vector<uint8_t> data;
};

namespace PacketSerializer
{
    // PlayerCommand
    ByteBuffer WriteCommand(const PlayerCommand& cmd);
    PlayerCommand ReadCommand(ByteBuffer& buf);

    // Server accept (seed, spawn pos, player ID)
    ByteBuffer WriteAccept(int playerId, int seed, const glm::vec3& spawnPos);
    void ReadAccept(ByteBuffer& buf, int& playerId, int& seed, glm::vec3& spawnPos);

    // Full snapshot of all players
    ByteBuffer WriteSnapshot(const std::vector<PlayerSnapshot>& snapshots);
    std::vector<PlayerSnapshot> ReadSnapshot(ByteBuffer& buf);

    // Block change
    ByteBuffer WriteBlockChange(const glm::ivec3& pos, BlockType type);
    BlockChange ReadBlockChange(ByteBuffer& buf);

    // Chunk data
    ByteBuffer WriteChunkData(const glm::ivec2& key, const std::vector<uint8_t>& chunkData);
    ChunkPacket ReadChunkData(ByteBuffer& buf);
}
