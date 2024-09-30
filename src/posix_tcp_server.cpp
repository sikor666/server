#include "posix/Session.h"

#include "ExecutionFactory.h"

#include <iostream>

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

constexpr auto PORT = "2345";  // default port users will be connecting to
constexpr auto BACKLOG = 4096; // how many pending connections queue will hold

// get sockaddr, IPv4 or IPv6:
void * get_in_addr(sockaddr * sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((sockaddr_in *)sa)->sin_addr);
    }

    return &(((sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    addrinfo hints{}, *servinfo{nullptr}, *p{nullptr};
    sockaddr_storage their_addr{}; // connector's address information
    socklen_t sin_size;
    int yes{1};
    char addr_str[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    int rv;
    if ((rv = getaddrinfo(nullptr, PORT, &hints, &servinfo)) != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
        std::exit(EXIT_FAILURE);
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != nullptr; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            std::perror("socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            std::perror("setsockopt");
            std::exit(EXIT_FAILURE);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            std::perror("bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == nullptr)
    {
        std::cerr << "server: failed to bind\n";
        std::exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        std::perror("listen");
        std::exit(EXIT_FAILURE);
    }

    // printf("server: waiting for connections...\n");

    std::shared_ptr<core::IExecutionPool> m_executionPool{core::CreateExecutionPool()};
    std::unique_ptr<core::IExecutionQueue<void(std::shared_ptr<session>)>> m_executionQueue{
        core::CreateConcurrentExecutionQueue<void, std::shared_ptr<session>>(m_executionPool,
            // Execution function is called in parallel on the next free thread with the next object from the queue
            [](const std::atomic_bool & isCanceled, std::shared_ptr<session> && session) {
                //   std::cout << "[" << std::this_thread::get_id() << "] start\n";
                session->start();
            })};

    while (true) // main accept loop
    {
        sin_size = sizeof(their_addr);
        new_fd = accept(sockfd, (sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            std::perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((sockaddr *)&their_addr), addr_str, sizeof(addr_str));
        // printf("server: got connection from %s\n", addr_str);

        m_executionQueue->push(std::make_shared<session>(std::move(new_fd)));
    }

    return EXIT_SUCCESS;
}