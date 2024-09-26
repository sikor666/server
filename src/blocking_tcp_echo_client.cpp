#include <boost/asio/connect.hpp>
#include "parallel/Client.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ts/buffer.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>

constexpr int max_length = 1024;

int main(int argc, char * argv[])
{
    try
    {
        std::cout << "Command: " << sizeof(Command) << "\n";
        std::cout << "End: " << sizeof(End) << "\n";

        if (argc != 3)
        {
            std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::ip::tcp::resolver resolver(io_context);
        boost::asio::connect(socket, resolver.resolve(argv[1], argv[2]));

        std::cout << "Enter message: ";
        char request[max_length];
        std::cin.getline(request, max_length);
        size_t request_length = std::strlen(request);
        boost::asio::write(socket, boost::asio::buffer(request, request_length));

        char reply[max_length];
        size_t reply_length = boost::asio::read(socket, boost::asio::buffer(reply, request_length));
        std::cout << "Reply is: ";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
