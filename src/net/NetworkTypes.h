#pragma once

#include <cstdint>

namespace Net
{
    constexpr uint16_t DefaultPort = 7777;
    constexpr int MaxClients = 16;
    constexpr int NumChannels = 2;

    // Channel 0: reliable (connect, disconnect, block changes, chat)
    // Channel 1: unreliable (player commands, state snapshots)
    constexpr int ChannelReliable = 0;
    constexpr int ChannelUnreliable = 1;

    enum class PacketType : uint8_t
    {
        // Client → Server
        ClientConnect = 1,
        ClientCommand,
        ClientDisconnect,

        // Server → Client
        ServerAccept,
        ServerReject,
        ServerSnapshot,
        ServerBlockChange,
        ServerPlayerJoined,
        ServerPlayerLeft,
        ServerChunkData,
    };
}
