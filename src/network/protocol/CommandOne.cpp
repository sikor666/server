#include "Command.h"

#include <iostream>

namespace network {
namespace protocol {
std::unique_ptr<ICommand> command::create(const std::string & payload)
{
    class CommandOne : public ICommand
    {
    public:
        CommandOne(const std::string & payload)
        {
            buffer::insert(m_buffer, Header{1, static_cast<int32_t>(payload.size())});
            buffer::insert(m_buffer, payload);
            buffer::insert(m_buffer, Footer{0});
        }

        virtual const std::vector<int8_t> & frame() const final { return m_buffer; }

        virtual void print() const final
        {
            const auto * header = reinterpret_cast<const network::protocol::Header *>(m_buffer.data());

            std::cout << "OK: " << reinterpret_cast<const char *>(header->payload) << "\n";
        }

    private:
        std::vector<int8_t> m_buffer;
    };

    return std::unique_ptr<ICommand>{new CommandOne{payload}};
}
} // namespace protocol
} // namespace network
