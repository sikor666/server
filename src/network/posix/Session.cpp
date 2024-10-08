#include "posix/Session.h"
#include "protocol/Command.h"

#include <cstdio>
#include <iostream>

#include <sys/socket.h>
#include <unistd.h>

constexpr auto MAXBUFFERSIZE = 1024; // max number of bytes we can get at once

Session::Session(int socket)
    : m_socket{socket}
{
}

Session::~Session()
{
    close(m_socket);
}

void Session::start()
{
    char buffer[MAXBUFFERSIZE];

    while (true)
    {
        int numbytes = recv(m_socket, buffer, MAXBUFFERSIZE, 0);

        if (numbytes == -1)
        {
            std::perror("recv");
            break;
        }
        else if (numbytes == 0)
        {
            // connection closed cleanly by peer
            break;
        }

        // got some data in buffer
        m_data.insert(m_data.end(), buffer, buffer + numbytes);
    }

    network::protocol::buffer::deserialize(m_data)->print();
}
