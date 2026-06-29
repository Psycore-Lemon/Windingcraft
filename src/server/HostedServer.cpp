#include "server/HostedServer.h"
#include "scene/Player.h"
#include "world/World.h"
#include "world/Chunk.h"
#include "world/ChunkSerializer.h"
#include "game/GameConfig.h"

#include <cstdio>
#include <cmath>

HostedServer::HostedServer() = default;
HostedServer::~HostedServer() { Stop(); }

bool HostedServer::Start(int seed, int loadRadius, uint16_t port)
{
    this->seed = seed;
    gameServer.Init(seed, loadRadius);

    if (!netServer.Start(port))
        return false;

    netServer.SetOnConnect([this](int peerId, const std::string& username) {
        OnPeerConnect(peerId, username);
    });
    netServer.SetOnDisconnect([this](int peerId) {
        OnPeerDisconnect(peerId);
    });
    netServer.SetOnData([this](int peerId, const uint8_t* data, size_t size) {
        OnPeerData(peerId, data, size);
    });

    return true;
}

void HostedServer::Stop()
{
    netServer.Stop();
}

bool HostedServer::IsRunning() const
{
    return netServer.IsRunning();
}

void HostedServer::SetSaveDir(const std::string& dir)
{
    gameServer.SetSaveDir(dir);
}

void HostedServer::Tick(float dt)
{
    netServer.Poll();
    gameServer.Tick(dt);
    BroadcastSnapshots();
    StreamChunks();
}

GameServer& HostedServer::GetGameServer()
{
    return gameServer;
}

const GameServer& HostedServer::GetGameServer() const
{
    return gameServer;
}

void HostedServer::StreamChunks()
{
    const World& world = gameServer.GetWorld();
    const auto& allChunks = world.GetChunks();
    int radius = GameConfig::ChunkLoadRadius;

    for (auto& [playerId, username] : playerUsernames)
    {
        auto peerIt = playerToPeer.find(playerId);
        if (peerIt == playerToPeer.end()) continue;
        int peerId = peerIt->second;

        const Player& player = gameServer.GetPlayer(playerId);
        int cx = (int)std::floor(player.position.x / (float)GameConfig::ChunkSize);
        int cz = (int)std::floor(player.position.z / (float)GameConfig::ChunkSize);

        auto& sent = sentChunks[peerId];

        for (int dx = -radius; dx <= radius; ++dx)
        {
            for (int dz = -radius; dz <= radius; ++dz)
            {
                glm::ivec2 key(cx + dx, cz + dz);
                if (sent.count(key)) continue;

                auto chunkIt = allChunks.find(key);
                if (chunkIt == allChunks.end()) continue;

                std::vector<uint8_t> data = ChunkSerializer::Serialize(*chunkIt->second);
                ByteBuffer buf = PacketSerializer::WriteChunkData(key, data);
                netServer.SendTo(peerId, buf, true);

                sent.insert(key);
            }
        }
    }
}

void HostedServer::OnPeerConnect(int peerId, const std::string& username)
{
    glm::vec3 spawnPos(0.0f, GameConfig::PlayerSpawnY, 0.0f);
    int playerId = gameServer.AddPlayer(spawnPos);

    peerToPlayer[peerId] = playerId;
    playerToPeer[playerId] = peerId;
    playerUsernames[playerId] = username;

    std::printf("[HostedServer] %s joined (peer=%d, player=%d)\n",
                username.c_str(), peerId, playerId);

    ByteBuffer accept = PacketSerializer::WriteAccept(playerId, seed, spawnPos);
    netServer.SendTo(peerId, accept, true);

    ByteBuffer joined;
    joined.WriteU8(static_cast<uint8_t>(Net::PacketType::ServerPlayerJoined));
    joined.WriteI32(playerId);
    joined.WriteString(username);
    netServer.Broadcast(joined, true);
}

void HostedServer::OnPeerDisconnect(int peerId)
{
    auto it = peerToPlayer.find(peerId);
    if (it == peerToPlayer.end()) return;

    int playerId = it->second;
    std::string username = playerUsernames[playerId];

    std::printf("[HostedServer] %s left (player=%d)\n", username.c_str(), playerId);

    gameServer.RemovePlayer(playerId);
    peerToPlayer.erase(peerId);
    playerToPeer.erase(playerId);
    playerUsernames.erase(playerId);
    sentChunks.erase(peerId);

    ByteBuffer left;
    left.WriteU8(static_cast<uint8_t>(Net::PacketType::ServerPlayerLeft));
    left.WriteI32(playerId);
    netServer.Broadcast(left, true);
}

void HostedServer::OnPeerData(int peerId, const uint8_t* data, size_t size)
{
    if (size < 1) return;

    auto packetType = static_cast<Net::PacketType>(data[0]);

    auto it = peerToPlayer.find(peerId);
    if (it == peerToPlayer.end()) return;
    int playerId = it->second;

    if (packetType == Net::PacketType::ClientCommand)
    {
        ByteBuffer buf(data + 1, size - 1);
        PlayerCommand cmd = PacketSerializer::ReadCommand(buf);
        gameServer.QueueCommand(playerId, cmd);
    }
}

void HostedServer::BroadcastSnapshots()
{
    std::vector<PlayerSnapshot> snapshots;

    for (auto& [playerId, username] : playerUsernames)
    {
        const Player& player = gameServer.GetPlayer(playerId);

        PlayerSnapshot snap;
        snap.playerId = playerId;
        snap.username = username;
        snap.position = player.position;
        snap.grounded = player.IsGrounded();
        snap.flying = player.IsFlying();
        snap.vitals = player.GetVitals();
        snap.hasTarget = player.HasTarget();
        snap.lookingAtBlock = player.GetTargetBlock();
        snap.lookingAtBlockPos = player.GetTargetBlockPos();
        snapshots.push_back(snap);
    }

    if (!snapshots.empty())
    {
        ByteBuffer buf = PacketSerializer::WriteSnapshot(snapshots);
        netServer.Broadcast(buf, false);
    }
}
