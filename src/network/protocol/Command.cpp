#include "Command.h"

#include <iostream>

namespace network {
namespace protocol {
std::unique_ptr<ICommand> command::error()
{
    class CommandError : public ICommand
    {
    public:
        CommandError()
        {
            buffer::insert(m_buffer, Header{0, 0});
            buffer::insert(m_buffer, Footer{0});
        }

        virtual const std::vector<int8_t> & frame() const final { return m_buffer; }
        virtual void print() const final { std::cerr << "ERROR\n"; }

    private:
        std::vector<int8_t> m_buffer;
    };

    return std::unique_ptr<ICommand>{new CommandError{}};
}

std::unique_ptr<ICommand> command::make(int32_t id, const std::vector<int8_t> & payload)
{
    switch (id)
    {
        case 1: return command::create(std::string{reinterpret_cast<const char *>(payload.data()), payload.size()});
        case 2: return command::create(*reinterpret_cast<const uint64_t *>(payload.data()));

        default: break;
    }

    return command::error();
}

std::unique_ptr<ICommand> buffer::deserialize(const std::vector<int8_t> & buffer)
{
    const Header * header = nullptr;

    for (size_t size = 0, state = 0; state < 3; state++)
    {
        switch (state)
        {
            case 0: {
                size += sizeof(Header);
            }
            break;
            case 1: {
                header = reinterpret_cast<const Header *>(buffer.data());
                size += header->length;
            }
            break;
            case 2: {
                if (reinterpret_cast<const Footer *>(buffer.data() + size)->end != 0)
                    return command::error();
                size += sizeof(Footer);
            }
            break;

            default: return command::error();
        }

        if (buffer.size() < size)
            return command::error();
    }

    return command::make(header->id, std::vector<int8_t>{header->payload, header->payload + header->length});
}

void buffer::insert(std::vector<int8_t> & buffer, const std::string & data)
{
    buffer.insert(buffer.end(), data.begin(), data.end());
}
} // namespace protocol
} // namespace network
