#pragma once

#include <enet/enet.h>
#include <string>
#include <functional>
#include <cstdint>

#include "net/NetworkTypes.h"
#include "net/ByteBuffer.h"

class NetClient
{
public:
    NetClient();
    ~NetClient();

    bool Connect(const std::string& host, uint16_t port, const std::string& username);
    void Disconnect();
    bool IsConnected() const;

    void Poll();

    void Send(const ByteBuffer& data, bool reliable);

    using ConnectedCallback = std::function<void()>;
    using DisconnectedCallback = std::function<void()>;
    using DataCallback = std::function<void(const uint8_t* data, size_t size)>;

    void SetOnConnected(ConnectedCallback cb) { onConnected = std::move(cb); }
    void SetOnDisconnected(DisconnectedCallback cb) { onDisconnected = std::move(cb); }
    void SetOnData(DataCallback cb) { onData = std::move(cb); }

private:
    ENetHost* client = nullptr;
    ENetPeer* server = nullptr;
    bool connected = false;
    std::string pendingUsername;

    ConnectedCallback onConnected;
    DisconnectedCallback onDisconnected;
    DataCallback onData;
};
