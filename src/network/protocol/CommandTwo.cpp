#include "Command.h"

#include <iostream>

namespace network {
namespace protocol {
std::unique_ptr<ICommand> command::create(const uint64_t & payload)
{
    class CommandTwo : public ICommand
    {
    public:
        CommandTwo(const uint64_t & payload)
        {
            buffer::insert(m_buffer, Header{2, sizeof(payload)});
            buffer::insert(m_buffer, payload);
            buffer::insert(m_buffer, Footer{0});
        }

        virtual const std::vector<int8_t> & frame() const final { return m_buffer; }

        virtual void print() const final
        {
            const auto * header = reinterpret_cast<const network::protocol::Header *>(m_buffer.data());
            const auto payload = *reinterpret_cast<const uint64_t *>(header->payload);

            std::cout << "OK: " << static_cast<const double>(payload) / 1000.0 << "\n";
        }

    private:
        std::vector<int8_t> m_buffer;
    };

    return std::unique_ptr<ICommand>{new CommandTwo{payload}};
}
} // namespace protocol
} // namespace network
