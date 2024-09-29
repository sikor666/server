#include "ExecutionFactory.h"
#include "Session.h"

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

#define PORT "2345" // the port users will be connecting to

#define BACKLOG 4096 // how many pending connections queue will hold

#define MAXDATASIZE 1024 // max number of bytes we can get at once

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, nullptr, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void * get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    int numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(nullptr, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != nullptr; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == nullptr)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, nullptr) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    std::shared_ptr<core::IExecutionPool> m_executionPool{core::CreateExecutionPool()};
    std::unique_ptr<core::IExecutionQueue<void(std::shared_ptr<session>)>> m_executionQueue{
        core::CreateConcurrentExecutionQueue<void, std::shared_ptr<session>>(m_executionPool,
            // Execution function is called in parallel on the next free thread with the next object from the queue
            [](const std::atomic_bool & isCanceled, std::shared_ptr<session> && session) {
                //   std::cout << "[" << std::this_thread::get_id() << "] start\n";
                session->start();
            })};

    while (true) // main accept() loop
    {
        sin_size = sizeof(their_addr);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
        // printf("server: got connection from %s\n", s);

        m_executionQueue->push(std::make_shared<session>(std::move(new_fd)));

        /*
        if (!fork())
        {                  // this is the child process
            close(sockfd); // child doesn't need the listener
            // if (send(new_fd, "Hello, world!", 13, 0) == -1)
            //     perror("send");
            if ((numbytes = recv(new_fd, buf, MAXDATASIZE - 1, 0)) == -1)
                perror("recv");

            buf[numbytes] = '\0';
            printf("server: received '%s'\n", buf);

            close(new_fd);
            exit(0);
        }
        close(new_fd); // parent doesn't need this
        */
    }

    return 0;
}
