#include "parallel/Session.h"
#include "protocol/Command.h"

#include <boost/asio/ts/buffer.hpp>

#include <iostream>
#include <thread>

constexpr auto max_length = 1024;

namespace network {
namespace parallel {
Session::Session(boost::asio::ip::tcp::socket socket)
    : m_socket(std::move(socket))
{
}

bool Session::read()
{
    for (;;)
    {
        char data[max_length];

        boost::system::error_code error;
        size_t length = m_socket.read_some(boost::asio::buffer(data), error);

        if (error == boost::asio::stream_errc::eof)
            return false; // Connection closed cleanly by peer
        else if (error)
            throw boost::system::system_error(error); // Some other error

        // boost::asio::write(m_socket, boost::asio::buffer(data, length));
        // std::cout << "Read: " << length << "\n";
        m_buffer.insert(m_buffer.end(), data, data + length);
    }

    return true;

    /*
    boost::system::error_code error;

    for (size_t size = 0, length = 0, state = 0; state < 3; state++)
    {
        switch (state)
        {
            // Read header
            case 0: size = sizeof(network::protocol::Header); break;
            // Read payload
            case 1: size = reinterpret_cast<const network::protocol::Header *>(m_buffer.data())->length; break;
            // Read footer
            case 2: size = sizeof(network::protocol::Footer); break;

            default: std::runtime_error("Invalid state");
        }

        m_buffer.resize(m_buffer.size() + size);

        do
        {
            length += m_socket.read_some(boost::asio::buffer(&m_buffer[length], size), error);

            if (error == boost::asio::stream_errc::eof)
                return false; // Connection closed cleanly by peer
            else if (error)
                throw boost::system::system_error(error); // Some other error
        } while (length < m_buffer.size());
    }

    return true;
    */
}

void Session::start()
{
    try
    {
        while (read())
        {
            network::protocol::buffer::deserialize(m_buffer)->print();
            boost::asio::write(m_socket, boost::asio::buffer(m_buffer));
        }

        // std::cerr << "[" << std::this_thread::get_id() << "] Connection closed cleanly by peer\n";
    }
    catch (const std::exception & ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
    }
}
} // namespace parallel
} // namespace network
