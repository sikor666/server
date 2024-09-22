#include "parallel/Server.h"

#include <iostream>

int main(int argc, char * argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: blocking_tcp_echo_server <port>\n";
            return 1;
        }

        network::parallel::Server server(std::atoi(argv[1]));
        server.run();
    }
    catch (std::exception & e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
