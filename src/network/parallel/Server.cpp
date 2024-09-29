#include "parallel/Server.h"
#include "parallel/Session.h"

#include "ExecutionFactory.h"

#include <iostream>
#include <memory>
#include <thread>

namespace network {
namespace parallel {
Server::Server(uint16_t port)
    : m_io_context{}
    , m_acceptor{m_io_context, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), port}}
    , m_executionPool{core::CreateExecutionPool()}
    , m_executionQueue{core::CreateConcurrentExecutionQueue<void, std::shared_ptr<Session>>(m_executionPool,
          // Execution function is called in parallel on the next free thread with the next object from the queue
          [](const std::atomic_bool & isCanceled, std::shared_ptr<Session> && session) {
              //   std::cout << "[" << std::this_thread::get_id() << "] start\n";
              session->start();
          })}
{
}

Server::~Server()
{
}

void Server::run()
{
    for (;;)
    {
        try
        {
            boost::asio::ip::tcp::socket sock{m_io_context};
            m_acceptor.accept(sock);

            // std::cout << "[" << std::this_thread::get_id() << "] push\n";
            m_executionQueue->push(std::make_shared<Session>(std::move(sock)));
        }
        catch (const std::exception & ex)
        {
            std::cerr << "Exception: " << ex.what() << "\n";
        }
    }
}
} // namespace parallel
} // namespace network
