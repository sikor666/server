#pragma once

#include <boost/asio/ts/internet.hpp>

namespace network {
namespace parallel {
class Server
{
public:
    Server(boost::asio::io_context & io_context, uint16_t port);
    ~Server();

private:
    /* data */
};
} // namespace parallel
} // namespace network
