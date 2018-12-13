#include "wrapsock.h"

#include <iostream>
#include <map>
#include <string>
#include <sstream>

#define	bzero(ptr,n) memset(ptr, 0, n)
constexpr auto SERV_PORT = 9877 /* TCP and UDP */;
#define	SA struct sockaddr
constexpr auto MAXLINE = 4096 /* max text line length */;

std::string players;
std::string ball;
std::string blocks;

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int n;
    socklen_t len;
    char mesg[MAXLINE];

    for (; ; ) {
        len = clilen;
        n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        mesg[n] = '\0';

        std::stringstream stream;
        stream << mesg;

        std::string line;
        std::getline(stream, line, '|');

        std::string message;

        if (line == "SET")
        {
            line = stream.str();
            auto found = line.find("|");
            if (found != std::string::npos)
            {
                players = line.substr(found + 1);
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

            std::cout << ball << std::endl;

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
        else if (line == "GET")
        {
            message = players;
        }
        else if (line == "GET_BLOCKS")
        {
            message = blocks;
        }
        else if (line == "GET_BALL")
        {
            message = ball;
        }

        Sendto(sockfd, message.c_str(), message.length(), 0, pcliaddr, len);
    }
}

int main(int argc, char **argv)
{
#if defined _WIN32
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
        printf("Initialization error.\n");
#endif

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(sockfd, (SA *)&servaddr, sizeof(servaddr));

    dg_echo(sockfd, (SA *)&cliaddr, sizeof(cliaddr));

#if defined _WIN32
    WSACleanup();
#endif
}
