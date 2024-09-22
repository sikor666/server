#pragma once

#include "IExecutionQueue.h"

#include <boost/asio/ts/internet.hpp>

namespace core {
/// @brief ThreadPool-like object that provides context for task execution.
class IExecutionPool;
} // namespace core

namespace network {
namespace async {
class Session;
class Server
{
public:
    Server(boost::asio::io_context & io_context, uint16_t port);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;

    std::shared_ptr<core::IExecutionPool> m_executionPool;
    std::unique_ptr<core::IExecutionQueue<void(std::shared_ptr<Session>)>> m_executionQueue;
};
} // namespace async
} // namespace network
