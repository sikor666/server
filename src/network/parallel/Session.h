#pragma once

#include <boost/asio/ip/tcp.hpp>

namespace network {
namespace parallel {
class Session
{
public:
    Session(boost::asio::ip::tcp::socket socket);

    void start();

private:
    bool read();

    boost::asio::ip::tcp::socket m_socket;
    std::vector<uint8_t> m_buffer;
};
} // namespace parallel
} // namespace network
