#include "parallel/Client.h"
#include "protocol/Command.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ts/buffer.hpp>

#include <iostream>

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
    }
    catch (const std::exception & ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
        std::terminate();
    }
}
} // namespace parallel
} // namespace network
