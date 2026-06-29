#include "net/NetClient.h"

#include <cstdio>

NetClient::NetClient()
{
    enet_initialize();
}

NetClient::~NetClient()
{
    Disconnect();
    enet_deinitialize();
}

bool NetClient::Connect(const std::string& host, uint16_t port, const std::string& username)
{
    client = enet_host_create(nullptr, 1, Net::NumChannels, 0, 0);
    if (!client)
    {
        std::printf("[NetClient] Failed to create client host\n");
        return false;
    }

    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    server = enet_host_connect(client, &address, Net::NumChannels, 0);
    if (!server)
    {
        std::printf("[NetClient] Failed to initiate connection\n");
        enet_host_destroy(client);
        client = nullptr;
        return false;
    }

    pendingUsername = username;

    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        connected = true;

        ByteBuffer buf;
        buf.WriteU8(static_cast<uint8_t>(Net::PacketType::ClientConnect));
        buf.WriteString(username);
        Send(buf, true);

        std::printf("[NetClient] Connected to %s:%d\n", host.c_str(), port);
        return true;
    }

    std::printf("[NetClient] Connection to %s:%d timed out\n", host.c_str(), port);
    enet_peer_reset(server);
    enet_host_destroy(client);
    client = nullptr;
    server = nullptr;
    return false;
}

void NetClient::Disconnect()
{
    if (!client) return;

    if (server && connected)
    {
        enet_peer_disconnect(server, 0);

        ENetEvent event;
        while (enet_host_service(client, &event, 1000) > 0)
        {
            if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                break;
            if (event.type == ENET_EVENT_TYPE_RECEIVE)
                enet_packet_destroy(event.packet);
        }
    }

    if (server)
        enet_peer_reset(server);

    enet_host_destroy(client);
    client = nullptr;
    server = nullptr;
    connected = false;
}

bool NetClient::IsConnected() const
{
    return connected;
}

void NetClient::Poll()
{
    if (!client) return;

    ENetEvent event;
    while (enet_host_service(client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            connected = true;
            if (onConnected) onConnected();
            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            connected = false;
            server = nullptr;
            if (onDisconnected) onDisconnected();
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            if (onData)
                onData(event.packet->data, event.packet->dataLength);
            enet_packet_destroy(event.packet);
            break;

        default:
            break;
        }
    }
}

void NetClient::Send(const ByteBuffer& data, bool reliable)
{
    if (!server || !connected) return;

    uint32_t flags = reliable ? ENET_PACKET_FLAG_RELIABLE : 0;
    int channel = reliable ? Net::ChannelReliable : Net::ChannelUnreliable;

    ENetPacket* packet = enet_packet_create(data.Data(), data.Size(), flags);
    enet_peer_send(server, channel, packet);
}
