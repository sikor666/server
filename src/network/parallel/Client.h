#pragma once

#include <boost/asio/ts/internet.hpp>

#pragma pack(push, 4)
struct Command
{
    int32_t id;
    int32_t length;
    uint8_t payload[0];
};
#pragma pack(pop)

constexpr int32_t End = 0;

namespace network {
namespace parallel {
class Client
{
public:
    Client(boost::asio::io_context & io_context);
    ~Client();

    void send(const std::string & host, const std::string & service, const std::string & command);

private:
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver resolver;
};
} // namespace parallel
} // namespace network
