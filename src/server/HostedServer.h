#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "server/GameServer.h"
#include "net/NetServer.h"
#include "net/PacketSerializer.h"

class HostedServer
{
public:
    HostedServer();
    ~HostedServer();

    bool Start(int seed, int loadRadius, uint16_t port = Net::DefaultPort);
    void Stop();
    bool IsRunning() const;

    void SetSaveDir(const std::string& dir);

    void Tick(float dt);

    GameServer& GetGameServer();
    const GameServer& GetGameServer() const;

private:
    void OnPeerConnect(int peerId, const std::string& username);
    void OnPeerDisconnect(int peerId);
    void OnPeerData(int peerId, const uint8_t* data, size_t size);

    void BroadcastSnapshots();

    GameServer gameServer;
    NetServer netServer;

    std::unordered_map<int, int> peerToPlayer;
    std::unordered_map<int, int> playerToPeer;
    std::unordered_map<int, std::string> playerUsernames;

    int seed = 0;
};
