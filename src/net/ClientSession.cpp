#include "net/ClientSession.h"
#include "world/World.h"
#include "world/Chunk.h"
#include "world/ChunkSerializer.h"

#include <cstdio>

ClientSession::ClientSession() = default;
ClientSession::~ClientSession() { Disconnect(); }

bool ClientSession::Connect(const std::string& host, uint16_t port, const std::string& username)
{
    client.SetOnData([this](const uint8_t* data, size_t size) {
        HandlePacket(data, size);
    });

    client.SetOnDisconnected([this]() {
        std::printf("[ClientSession] Disconnected from server\n");
        accepted = false;
    });

    if (!client.Connect(host, port, username))
        return false;

    return true;
}

void ClientSession::Disconnect()
{
    client.Disconnect();
    accepted = false;
    playerId = -1;
}

bool ClientSession::IsConnected() const
{
    return client.IsConnected();
}

void ClientSession::Poll()
{
    client.Poll();
}

void ClientSession::SendCommand(const PlayerCommand& cmd)
{
    if (!accepted) return;
    ByteBuffer buf = PacketSerializer::WriteCommand(cmd);
    client.Send(buf, false);
}

bool ClientSession::IsAccepted() const { return accepted; }
int ClientSession::GetPlayerId() const { return playerId; }
int ClientSession::GetSeed() const { return seed; }
const glm::vec3& ClientSession::GetSpawnPosition() const { return spawnPos; }

const PlayerSnapshot& ClientSession::GetLocalSnapshot() const { return localSnapshot; }
const std::vector<PlayerSnapshot>& ClientSession::GetAllSnapshots() const { return allSnapshots; }

int ClientSession::GetPing() const
{
    return client.GetPing();
}

void ClientSession::ApplyWorldChanges(World& world)
{
    for (const auto& bc : pendingBlockChanges)
        world.SetBlock(glm::vec3(bc.position), bc.type);
    pendingBlockChanges.clear();
}

void ClientSession::ApplyChunkData(World& world)
{
    for (const auto& cp : pendingChunks)
    {
        auto chunk = std::make_unique<Chunk>(cp.key.x, cp.key.y);
        ChunkSerializer::Deserialize(*chunk, cp.data);
        world.InjectChunk(cp.key, std::move(chunk));
    }
    pendingChunks.clear();
}

void ClientSession::HandlePacket(const uint8_t* data, size_t size)
{
    if (size < 1) return;

    auto type = static_cast<Net::PacketType>(data[0]);
    ByteBuffer buf(data + 1, size - 1);

    switch (type)
    {
    case Net::PacketType::ServerAccept:
    {
        PacketSerializer::ReadAccept(buf, playerId, seed, spawnPos);
        accepted = true;
        std::printf("[ClientSession] Accepted by server (id=%d, seed=%d)\n", playerId, seed);
        break;
    }

    case Net::PacketType::ServerSnapshot:
    {
        allSnapshots = PacketSerializer::ReadSnapshot(buf);
        for (const auto& snap : allSnapshots)
        {
            if (snap.playerId == playerId)
            {
                localSnapshot = snap;
                break;
            }
        }
        break;
    }

    case Net::PacketType::ServerBlockChange:
    {
        pendingBlockChanges.push_back(PacketSerializer::ReadBlockChange(buf));
        break;
    }

    case Net::PacketType::ServerChunkData:
    {
        pendingChunks.push_back(PacketSerializer::ReadChunkData(buf));
        break;
    }

    case Net::PacketType::ServerPlayerJoined:
    {
        int id = buf.ReadI32();
        std::string username = buf.ReadString();
        std::printf("[ClientSession] %s joined (id=%d)\n", username.c_str(), id);
        break;
    }

    case Net::PacketType::ServerPlayerLeft:
    {
        int id = buf.ReadI32();
        std::printf("[ClientSession] Player %d left\n", id);
        break;
    }

    default:
        break;
    }
}
