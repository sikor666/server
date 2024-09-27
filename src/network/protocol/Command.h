#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace network {
namespace protocol {
#pragma pack(push, 4)
struct Header
{
    int32_t id;
    int32_t length;
    uint8_t payload[0];
};

struct Footer
{
    int32_t end;
};
#pragma pack(pop)

class ICommand
{
public:
    virtual ~ICommand() = default;

    virtual const std::vector<uint8_t> & frame() const = 0;
};

class CommandOne : public ICommand
{
public:
    CommandOne(const std::string & payload);

    virtual const std::vector<uint8_t> & frame() const final;

private:
    std::vector<uint8_t> m_buffer;
};

namespace buffer {
void insert(std::vector<uint8_t> & buffer, const std::string & data);
// FIXME: Use decorator, enable_if and specialization for string
template <typename T, typename = std::enable_if_t<std::is_trivial<T>::value and not std::is_pointer<T>::value>>
void insert(std::vector<uint8_t> & buffer, T && data)
{
    std::cout << "typename: " << typeid(data).name() << "\n";

    const auto * begin = reinterpret_cast<const uint8_t *>(&data);

    buffer.insert(buffer.end(), begin, begin + sizeof(data));
}
} // namespace buffer
} // namespace protocol
} // namespace network
