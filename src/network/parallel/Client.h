#pragma once

#include <boost/asio/ts/internet.hpp>

class Client
{
public:
    Client(boost::asio::io_context & io_context);
    ~Client();

private:
    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::resolver resolver;
};
