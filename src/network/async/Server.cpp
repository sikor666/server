#include "async/Server.h"
#include "async/Session.h"

#include "ExecutionFactory.h"

namespace network {
namespace async {
Server::Server(boost::asio::io_context & io_context, uint16_t port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), port})
    , socket_(io_context)
    , m_executionPool{core::CreateExecutionPool()}
    , m_executionQueue{core::CreateConcurrentExecutionQueue<void, std::shared_ptr<Session>>(m_executionPool,
          // Execution function is called in parallel on the next free thread with the next object from the queue
          [](const std::atomic_bool & isCanceled, std::shared_ptr<Session> && object) { object->start(); })}
{
    do_accept();
}

void Server::do_accept()
{
    acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
        if (!ec)
        {
            m_executionQueue->push(std::make_shared<Session>(std::move(socket_)));
        }

        do_accept();
    });
}
} // namespace async
} // namespace network
