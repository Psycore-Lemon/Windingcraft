#include "net/PacketSerializer.h"

ByteBuffer PacketSerializer::WriteCommand(const PlayerCommand& cmd)
{
    ByteBuffer buf;
    buf.WriteU8(static_cast<uint8_t>(Net::PacketType::ClientCommand));
    buf.WriteVec3(cmd.moveDirection);
    buf.WriteVec3(cmd.lookDirection);

    uint8_t flags = 0;
    if (cmd.jump)            flags |= (1 << 0);
    if (cmd.descend)         flags |= (1 << 1);
    if (cmd.primaryAction)   flags |= (1 << 2);
    if (cmd.secondaryAction) flags |= (1 << 3);
    buf.WriteU8(flags);

    buf.WriteFloat(cmd.scrollDelta);
    buf.WriteI32(cmd.hotbarSelect);
    return buf;
}

PlayerCommand PacketSerializer::ReadCommand(ByteBuffer& buf)
{
    PlayerCommand cmd;
    cmd.moveDirection = buf.ReadVec3();
    cmd.lookDirection = buf.ReadVec3();

    uint8_t flags = buf.ReadU8();
    cmd.jump            = (flags & (1 << 0)) != 0;
    cmd.descend         = (flags & (1 << 1)) != 0;
    cmd.primaryAction   = (flags & (1 << 2)) != 0;
    cmd.secondaryAction = (flags & (1 << 3)) != 0;

    cmd.scrollDelta = buf.ReadFloat();
    cmd.hotbarSelect = buf.ReadI32();
    return cmd;
}

ByteBuffer PacketSerializer::WriteAccept(int playerId, int seed, const glm::vec3& spawnPos)
{
    ByteBuffer buf;
    buf.WriteU8(static_cast<uint8_t>(Net::PacketType::ServerAccept));
    buf.WriteI32(playerId);
    buf.WriteI32(seed);
    buf.WriteVec3(spawnPos);
    return buf;
}

void PacketSerializer::ReadAccept(ByteBuffer& buf, int& playerId, int& seed, glm::vec3& spawnPos)
{
    playerId = buf.ReadI32();
    seed = buf.ReadI32();
    spawnPos = buf.ReadVec3();
}

ByteBuffer PacketSerializer::WriteSnapshot(const std::vector<PlayerSnapshot>& snapshots)
{
    ByteBuffer buf;
    buf.WriteU8(static_cast<uint8_t>(Net::PacketType::ServerSnapshot));
    buf.WriteU16(static_cast<uint16_t>(snapshots.size()));

    for (const auto& s : snapshots)
    {
        buf.WriteI32(s.playerId);
        buf.WriteString(s.username);
        buf.WriteVec3(s.position);

        uint8_t flags = 0;
        if (s.grounded)  flags |= (1 << 0);
        if (s.flying)    flags |= (1 << 1);
        if (s.hasTarget) flags |= (1 << 2);
        buf.WriteU8(flags);

        buf.WriteFloat(s.vitals.health.current);
        buf.WriteFloat(s.vitals.health.max);
        buf.WriteFloat(s.vitals.mana.current);
        buf.WriteFloat(s.vitals.mana.max);
        buf.WriteFloat(s.vitals.stamina.current);
        buf.WriteFloat(s.vitals.stamina.max);

        buf.WriteU16(static_cast<uint16_t>(s.lookingAtBlock));
        buf.WriteIVec3(s.lookingAtBlockPos);
    }

    return buf;
}

std::vector<PlayerSnapshot> PacketSerializer::ReadSnapshot(ByteBuffer& buf)
{
    uint16_t count = buf.ReadU16();
    std::vector<PlayerSnapshot> snapshots(count);

    for (int i = 0; i < count; ++i)
    {
        auto& s = snapshots[i];
        s.playerId = buf.ReadI32();
        s.username = buf.ReadString();
        s.position = buf.ReadVec3();

        uint8_t flags = buf.ReadU8();
        s.grounded  = (flags & (1 << 0)) != 0;
        s.flying    = (flags & (1 << 1)) != 0;
        s.hasTarget = (flags & (1 << 2)) != 0;

        s.vitals.health.current  = buf.ReadFloat();
        s.vitals.health.max      = buf.ReadFloat();
        s.vitals.mana.current    = buf.ReadFloat();
        s.vitals.mana.max        = buf.ReadFloat();
        s.vitals.stamina.current = buf.ReadFloat();
        s.vitals.stamina.max     = buf.ReadFloat();

        s.lookingAtBlock = static_cast<BlockType>(buf.ReadU16());
        s.lookingAtBlockPos = buf.ReadIVec3();
    }

    return snapshots;
}

ByteBuffer PacketSerializer::WriteBlockChange(const glm::ivec3& pos, BlockType type)
{
    ByteBuffer buf;
    buf.WriteU8(static_cast<uint8_t>(Net::PacketType::ServerBlockChange));
    buf.WriteIVec3(pos);
    buf.WriteU16(static_cast<uint16_t>(type));
    return buf;
}

BlockChange PacketSerializer::ReadBlockChange(ByteBuffer& buf)
{
    BlockChange bc;
    bc.position = buf.ReadIVec3();
    bc.type = static_cast<BlockType>(buf.ReadU16());
    return bc;
}

ByteBuffer PacketSerializer::WriteChunkData(const glm::ivec2& key, const std::vector<uint8_t>& chunkData)
{
    ByteBuffer buf;
    buf.WriteU8(static_cast<uint8_t>(Net::PacketType::ServerChunkData));
    buf.WriteI32(key.x);
    buf.WriteI32(key.y);
    buf.WriteU32(static_cast<uint32_t>(chunkData.size()));
    buf.WriteBytes(chunkData.data(), chunkData.size());
    return buf;
}

ChunkPacket PacketSerializer::ReadChunkData(ByteBuffer& buf)
{
    ChunkPacket cp;
    cp.key.x = buf.ReadI32();
    cp.key.y = buf.ReadI32();
    uint32_t size = buf.ReadU32();
    cp.data.resize(size);
    buf.ReadBytes(cp.data.data(), size);
    return cp;
}
