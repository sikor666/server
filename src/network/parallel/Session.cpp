#include "parallel/Session.h"

#include <boost/asio/ts/buffer.hpp>

#include <iostream>
#include <thread>

constexpr int max_length = 1024;

namespace network {
namespace parallel {
Session::Session(boost::asio::ip::tcp::socket socket)
    : m_socket(std::move(socket))
{
}

void Session::operator()()
{
    try
    {
        for (;;)
        {
            char data[max_length];

            boost::system::error_code error;
            size_t length = m_socket.read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::stream_errc::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.

            std::cout << "[" << std::this_thread::get_id() << "] " << std::string{data, length};
            boost::asio::write(m_socket, boost::asio::buffer(data, length));
        }

        std::cout << "[" << std::this_thread::get_id() << "] Connection closed cleanly by peer\n";
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}
} // namespace parallel
} // namespace network
