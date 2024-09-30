#pragma once

#include <memory>
#include <string>
#include <vector>

namespace network {
namespace protocol {
#pragma pack(push, 4)
struct Header
{
    int32_t id;
    int32_t length;
    int8_t payload[0];
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

    virtual const std::vector<int8_t> & frame() const = 0;
    virtual void print() const = 0;
};

namespace command {
std::unique_ptr<ICommand> error();
std::unique_ptr<ICommand> create(const std::string & payload);
std::unique_ptr<ICommand> create(const uint64_t & payload);
std::unique_ptr<ICommand> make(int32_t id, const std::vector<int8_t> & payload);
} // namespace command

namespace buffer {
std::unique_ptr<ICommand> deserialize(const std::vector<int8_t> & buffer);

void insert(std::vector<int8_t> & buffer, const std::string & data);
template <typename T, typename = std::enable_if_t<std::is_trivial<T>::value and not std::is_pointer<T>::value>>
void insert(std::vector<int8_t> & buffer, const T & data)
{
    const auto * begin = reinterpret_cast<const uint8_t *>(&data);
    buffer.insert(buffer.end(), begin, begin + sizeof(T));
}
} // namespace buffer
} // namespace protocol
} // namespace network
