#pragma once

#include <enet/enet.h>
#include <unordered_map>
#include <string>
#include <functional>
#include <cstdint>

#include "net/NetworkTypes.h"
#include "net/ByteBuffer.h"

struct NetPeer
{
    ENetPeer* peer = nullptr;
    int playerId = -1;
    std::string username;
};

class NetServer
{
public:
    NetServer();
    ~NetServer();

    bool Start(uint16_t port = Net::DefaultPort);
    void Stop();
    bool IsRunning() const;

    void Poll();

    void SendTo(int playerId, const ByteBuffer& data, bool reliable);
    void Broadcast(const ByteBuffer& data, bool reliable);

    using ConnectCallback = std::function<void(int peerId, const std::string& username)>;
    using DisconnectCallback = std::function<void(int peerId)>;
    using DataCallback = std::function<void(int peerId, const uint8_t* data, size_t size)>;

    void SetOnConnect(ConnectCallback cb) { onConnect = std::move(cb); }
    void SetOnDisconnect(DisconnectCallback cb) { onDisconnect = std::move(cb); }
    void SetOnData(DataCallback cb) { onData = std::move(cb); }

private:
    ENetHost* host = nullptr;
    std::unordered_map<int, NetPeer> peers;
    int nextPeerId = 0;

    ConnectCallback onConnect;
    DisconnectCallback onDisconnect;
    DataCallback onData;
};
