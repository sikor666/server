#include "parallel/Client.h"
#include "protocol/Command.h"

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

void Client::send(const std::string & host, const std::string & service, const std::vector<int8_t> & command)
{
    try
    {
        boost::asio::connect(socket, resolver.resolve(host, service));
        boost::asio::write(socket, boost::asio::buffer(command));

        std::cout << "Write: " << command.size() << "\n";

        // char reply[max_length];
        // size_t reply_length = boost::asio::read(socket, boost::asio::buffer(reply, command.size()));

        // Reply is:
        // network::protocol::buffer::deserialize(std::vector<int8_t>{reply, reply + reply_length})->print();
    }
    catch (const std::exception & ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
        std::terminate();
    }
}
} // namespace parallel
} // namespace network
