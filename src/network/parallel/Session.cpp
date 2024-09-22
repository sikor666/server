#include "parallel/Session.h"

namespace network {
namespace parallel {
Session::Session(boost::asio::ip::tcp::socket socket)
    : m_socket(std::move(socket))
{
}

Session::~Session()
{
}
} // namespace parallel
} // namespace network
