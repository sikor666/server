#include "async/Session.h"

#include <boost/asio/ts/buffer.hpp>

#include <iostream>

namespace network {
namespace async {
Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket))
{
}

void Session::start()
{
    do_read();
}

void Session::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_, max_length), [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec)
            {
                std::cout << std::string{data_, length};
                do_write(length);
            }
        });
}

void Session::do_write(std::size_t length)
{
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_, boost::asio::buffer(data_, length), [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec)
            {
                do_read();
            }
        });
}
} // namespace async
} // namespace network
