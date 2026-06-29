#pragma once

#include <string>
#include <memory>
#include <cstdint>
#include <glm/glm.hpp>

#include "net/NetClient.h"
#include "net/PacketSerializer.h"
#include "game/PlayerCommand.h"
#include "game/Resource.h"

class World;

class ClientSession
{
public:
    ClientSession();
    ~ClientSession();

    bool Connect(const std::string& host, uint16_t port, const std::string& username);
    void Disconnect();
    bool IsConnected() const;

    void Poll();
    void SendCommand(const PlayerCommand& cmd);

    bool IsAccepted() const;
    int GetPlayerId() const;
    int GetSeed() const;
    const glm::vec3& GetSpawnPosition() const;

    const PlayerSnapshot& GetLocalSnapshot() const;
    const std::vector<PlayerSnapshot>& GetAllSnapshots() const;

    void ApplyWorldChanges(World& world);
    void ApplyChunkData(World& world);

    int GetPing() const;

private:
    void HandlePacket(const uint8_t* data, size_t size);

    NetClient client;
    bool accepted = false;

    int playerId = -1;
    int seed = 0;
    glm::vec3 spawnPos{0.0f};

    PlayerSnapshot localSnapshot;
    std::vector<PlayerSnapshot> allSnapshots;
    std::vector<BlockChange> pendingBlockChanges;
    std::vector<ChunkPacket> pendingChunks;
};
