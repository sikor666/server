#include "Client.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ts/buffer.hpp>

#include <iostream>

constexpr int max_length = 1024;

namespace network {
namespace parallel {
Client::Client(boost::asio::io_context & io_context)
    : socket{io_context}
    , resolver{io_context}
{
}

void Client::send(const std::string & host, const std::string & service, const std::string & command)
{
    try
    {
        boost::asio::connect(socket, resolver.resolve(host, service));
        boost::asio::write(socket, boost::asio::buffer(command, command.size()));

        char reply[max_length];
        size_t reply_length = boost::asio::read(socket, boost::asio::buffer(reply, command.size()));

        std::cout << "Reply is: " << std::string{reply, reply_length} << "\n";
    }
    catch (const std::exception & ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
        std::terminate();
    }
}
} // namespace parallel
} // namespace network
