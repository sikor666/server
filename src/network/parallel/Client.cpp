#include "Client.h"

Client::Client(boost::asio::io_context & io_context)
    : socket{io_context}
    , resolver{io_context}
{
}

Client::~Client()
{
}
