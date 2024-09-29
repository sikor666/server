#include "async/Server.h"

#include <iostream>

int main(int argc, char * argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        network::async::Server s(io_context, std::atoi(argv[1]));

        io_context.run();
    }
    catch (const std::exception & ex)
    {
        std::cerr << "Exception: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
