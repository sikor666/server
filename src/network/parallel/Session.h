#pragma once

#include <boost/asio/ip/tcp.hpp>

namespace network {
namespace parallel {
class Session
{
public:
    Session(boost::asio::ip::tcp::socket socket);
    ~Session();

private:
    boost::asio::ip::tcp::socket m_socket;
};
} // namespace parallel
} // namespace network
