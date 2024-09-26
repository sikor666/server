#pragma once

#include <cstdint>
#include <vector>

namespace network {
#pragma pack(push, 4)
struct Command
{
    int32_t id;
    int32_t length;
    uint8_t payload[0];
};
#pragma pack(pop)

constexpr int32_t End = 0;

template <typename Data> void bufferInsert(std::vector<uint8_t> & buffer, const Data & data)
{
    buffer.insert(
        buffer.begin(), reinterpret_cast<const uint8_t *>(&data), reinterpret_cast<const uint8_t *>(&data) + sizeof(data));
}
} // namespace network
