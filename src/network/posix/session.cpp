#include "session.h"

#include <cstdio>

#include <sys/socket.h>
#include <unistd.h>

#define MAXDATASIZE 1024 // max number of bytes we can get at once

session::session(int socket)
    : m_socket{socket}
{
}

session::~session()
{
    close(m_socket);
}

void session::start()
{
    int numbytes;
    char buf[MAXDATASIZE];

    if ((numbytes = recv(m_socket, buf, MAXDATASIZE - 1, 0)) == -1)
        perror("recv");

    buf[numbytes] = '\0';
    // printf("server: received '%s'\n", buf);

    usleep(500);
}
