#pragma once

#include <boost/asio/ts/internet.hpp>

namespace network {
namespace parallel {
class Client
{
public:
    Client(boost::asio::io_context & io_context);

    void send(const std::string & host, const std::string & service, const std::vector<int8_t> & command);

private:
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver resolver;
};
} // namespace parallel
} // namespace network
