#pragma once

#include "wrapsock.h"

#include <array>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

#define	bzero(ptr,n) memset(ptr, 0, n)
#define	SA struct sockaddr

constexpr auto SERV_PORT = 9877 /* TCP and UDP */;
constexpr auto MAXLINE = 4096 /* max text line length */;
constexpr auto MAXCLIENTS = 4;

std::array<std::string, MAXCLIENTS> clients;
std::map<std::string, std::string> players;
std::string ball;
std::string blocks;

void err_quit(const char* c)
{
    perror(c);
    exit(1);
}

char *sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char portstr[8];
    static char str[128];

    switch (sa->sa_family)
    {
    case AF_INET: {
        struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

        if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
        {
            return NULL;
        }

        if (ntohs(sin->sin_port) != 0)
        {
            snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
            strcat(str, portstr);
        }

        return str;
    }
    default:
        snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
        return str;
    }

    return NULL;
}

char *Sock_ntop(const struct sockaddr *sa, socklen_t salen)
{
    char *ptr;

    if ((ptr = sock_ntop(sa, salen)) == NULL)
        err_quit("sock_ntop error"); // inet_ntop() sets errno

    return ptr;
}

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char mesg[MAXLINE];

    while (true)
    {
        len = clilen;
        n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        mesg[n] = '\0';
        printf("request from %s\n", Sock_ntop(pcliaddr, len));


        //std::string respone = parseMessage(mesg, n);

        std::stringstream stream;
        stream << mesg;

        std::string line;
        std::getline(stream, line, '|');

        std::string message;

        if (line == "CONNECT")
        {
            for (auto client : clients)
            {

            }
        }
        else if (line == "SET_PLAYER")
        {
            if (std::getline(stream, line, '|'))
            {
                auto found = line.find("$");
                if (found != std::string::npos)
                {
                    std::string index = line.substr(0, found);
                    std::string left = line.substr(found + 1);

                    players[index] = left;
                }
            }

            //message = "SET:OK";
        }
        else if (line == "SET_BALL")
        {
            line = stream.str();
            auto found = line.find("|");
            if (found != std::string::npos)
            {
                ball = line.substr(found + 1);
            }

            //message = "SET_BALL:OK";
        }
        else if (line == "SET_BLOCKS")
        {
            line = stream.str();
            auto found = line.find("|");
            if (found != std::string::npos)
            {
                blocks = line.substr(found + 1);
            }

            //message = "BLOCKS:OK";
        }
        else if (line == "GET_PLAYERS")
        {
            for (auto player : players)
            {
                message += player.first + "$" + player.second + "|";
            }
        }
        else if (line == "GET_BLOCKS")
        {
            message = blocks;
        }
        else if (line == "GET_BALL")
        {
            message = ball;
        }
        else
        {
            message = mesg;
        }

        Sendto(sockfd, message.c_str(), message.length(), 0, pcliaddr, len);
    }
}

class Server
{
public:
    Server()
    {
#if defined _WIN32
        WSADATA wsaData;

        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != NO_ERROR)
            printf("Initialization error.\n");
#endif

        sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(SERV_PORT);

        Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));
    }

    ~Server()
    {
#if defined _WIN32
        WSACleanup();
#endif
    }

    void run()
    {
        dg_echo(sockfd, (SA *)&cliaddr, sizeof(cliaddr));
    }

private:
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
};
