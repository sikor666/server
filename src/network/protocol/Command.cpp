#include "Command.h"

namespace network {
namespace protocol {
CommandOne::CommandOne(const std::string & payload)
{
    buffer::insert(m_buffer, Header{1, static_cast<int32_t>(payload.size())});
    buffer::insert(m_buffer, payload);
    buffer::insert(m_buffer, Footer{0});
}

const std::vector<uint8_t> & CommandOne::frame() const
{
    return m_buffer;
}

namespace buffer {
void insert(std::vector<uint8_t> & buffer, const std::string & data)
{
    std::cout << "string: " << typeid(data).name() << "\n";

    buffer.insert(buffer.end(), data.begin(), data.end());
}
} // namespace buffer
} // namespace protocol
} // namespace network
