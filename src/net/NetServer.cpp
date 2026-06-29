#include "net/NetServer.h"

#include <cstdio>

NetServer::NetServer()
{
    enet_initialize();
}

NetServer::~NetServer()
{
    Stop();
    enet_deinitialize();
}

bool NetServer::Start(uint16_t port)
{
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    host = enet_host_create(&address, Net::MaxClients, Net::NumChannels, 0, 0);
    if (!host)
    {
        std::printf("[NetServer] Failed to create host on port %d\n", port);
        return false;
    }

    std::printf("[NetServer] Listening on port %d\n", port);
    return true;
}

void NetServer::Stop()
{
    if (!host) return;

    for (auto& [id, peer] : peers)
    {
        if (peer.peer)
            enet_peer_disconnect_now(peer.peer, 0);
    }
    peers.clear();

    enet_host_destroy(host);
    host = nullptr;
}

bool NetServer::IsRunning() const
{
    return host != nullptr;
}

void NetServer::Poll()
{
    if (!host) return;

    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            int id = nextPeerId++;
            NetPeer np;
            np.peer = event.peer;
            np.playerId = id;
            peers[id] = np;
            event.peer->data = reinterpret_cast<void*>(static_cast<intptr_t>(id));
            std::printf("[NetServer] Peer connected (id=%d)\n", id);
            break;
        }

        case ENET_EVENT_TYPE_DISCONNECT:
        {
            int id = static_cast<int>(reinterpret_cast<intptr_t>(event.peer->data));
            std::printf("[NetServer] Peer disconnected (id=%d)\n", id);
            if (onDisconnect) onDisconnect(id);
            peers.erase(id);
            break;
        }

        case ENET_EVENT_TYPE_RECEIVE:
        {
            int id = static_cast<int>(reinterpret_cast<intptr_t>(event.peer->data));

            if (event.packet->dataLength > 0)
            {
                uint8_t type = event.packet->data[0];

                if (type == static_cast<uint8_t>(Net::PacketType::ClientConnect))
                {
                    ByteBuffer buf(event.packet->data + 1, event.packet->dataLength - 1);
                    std::string username = buf.ReadString();
                    peers[id].username = username;
                    if (onConnect) onConnect(id, username);
                }
                else if (onData)
                {
                    onData(id, event.packet->data, event.packet->dataLength);
                }
            }

            enet_packet_destroy(event.packet);
            break;
        }

        default:
            break;
        }
    }
}

void NetServer::SendTo(int playerId, const ByteBuffer& data, bool reliable)
{
    auto it = peers.find(playerId);
    if (it == peers.end() || !it->second.peer) return;

    uint32_t flags = reliable ? ENET_PACKET_FLAG_RELIABLE : 0;
    int channel = reliable ? Net::ChannelReliable : Net::ChannelUnreliable;

    ENetPacket* packet = enet_packet_create(data.Data(), data.Size(), flags);
    enet_peer_send(it->second.peer, channel, packet);
}

void NetServer::Broadcast(const ByteBuffer& data, bool reliable)
{
    if (!host) return;

    uint32_t flags = reliable ? ENET_PACKET_FLAG_RELIABLE : 0;
    int channel = reliable ? Net::ChannelReliable : Net::ChannelUnreliable;

    ENetPacket* packet = enet_packet_create(data.Data(), data.Size(), flags);
    enet_host_broadcast(host, channel, packet);
}
