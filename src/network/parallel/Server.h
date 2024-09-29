#pragma once

#include "IExecutionQueue.h"

#include <boost/asio/ts/internet.hpp>

namespace core {
/// @brief ThreadPool-like object that provides context for task execution.
class IExecutionPool;
} // namespace core

namespace network {
namespace parallel {
class Session;
class Server
{
public:
    Server(uint16_t port);

    void run();

private:
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::acceptor m_acceptor;

    std::shared_ptr<core::IExecutionPool> m_executionPool;
    std::unique_ptr<core::IExecutionQueue<void(std::shared_ptr<Session>)>> m_executionQueue;
};
} // namespace parallel
} // namespace network
