#pragma once

#include <vector>
#include <cstring>
#include <string>
#include <cstdint>
#include <glm/glm.hpp>

class ByteBuffer
{
public:
    ByteBuffer() = default;
    ByteBuffer(const uint8_t* data, size_t size)
        : buffer(data, data + size) {}

    // Write
    void WriteU8(uint8_t v) { buffer.push_back(v); }
    void WriteU16(uint16_t v) { Write(&v, sizeof(v)); }
    void WriteU32(uint32_t v) { Write(&v, sizeof(v)); }
    void WriteI32(int32_t v) { Write(&v, sizeof(v)); }
    void WriteFloat(float v) { Write(&v, sizeof(v)); }

    void WriteVec3(const glm::vec3& v)
    {
        WriteFloat(v.x);
        WriteFloat(v.y);
        WriteFloat(v.z);
    }

    void WriteIVec3(const glm::ivec3& v)
    {
        WriteI32(v.x);
        WriteI32(v.y);
        WriteI32(v.z);
    }

    void WriteString(const std::string& s)
    {
        WriteU16(static_cast<uint16_t>(s.size()));
        buffer.insert(buffer.end(), s.begin(), s.end());
    }

    void WriteBytes(const uint8_t* data, size_t len)
    {
        buffer.insert(buffer.end(), data, data + len);
    }

    // Read
    uint8_t ReadU8() { uint8_t v; Read(&v, sizeof(v)); return v; }
    uint16_t ReadU16() { uint16_t v; Read(&v, sizeof(v)); return v; }
    uint32_t ReadU32() { uint32_t v; Read(&v, sizeof(v)); return v; }
    int32_t ReadI32() { int32_t v; Read(&v, sizeof(v)); return v; }
    float ReadFloat() { float v; Read(&v, sizeof(v)); return v; }

    glm::vec3 ReadVec3()
    {
        float x = ReadFloat(), y = ReadFloat(), z = ReadFloat();
        return glm::vec3(x, y, z);
    }

    glm::ivec3 ReadIVec3()
    {
        int x = ReadI32(), y = ReadI32(), z = ReadI32();
        return glm::ivec3(x, y, z);
    }

    std::string ReadString()
    {
        uint16_t len = ReadU16();
        std::string s(buffer.begin() + readPos, buffer.begin() + readPos + len);
        readPos += len;
        return s;
    }

    void ReadBytes(uint8_t* out, size_t len)
    {
        Read(out, len);
    }

    const uint8_t* Data() const { return buffer.data(); }
    size_t Size() const { return buffer.size(); }
    size_t Remaining() const { return buffer.size() - readPos; }

private:
    void Write(const void* data, size_t size)
    {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        buffer.insert(buffer.end(), bytes, bytes + size);
    }

    void Read(void* out, size_t size)
    {
        std::memcpy(out, buffer.data() + readPos, size);
        readPos += size;
    }

    std::vector<uint8_t> buffer;
    size_t readPos = 0;
};
