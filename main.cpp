#include "wrapsock.h"

#include <iostream>
#include <map>
#include <string>
#include <sstream>

#define	bzero(ptr,n) memset(ptr, 0, n)
constexpr auto SERV_PORT = 9877 /* TCP and UDP */;
#define	SA struct sockaddr
constexpr auto MAXLINE = 4096 /* max text line length */;

std::map<size_t, float> playersLeft;
//std::pair<float, float> ballLeftTop;
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
            while (std::getline(stream, line, '|'))
            {
                auto found = line.find("#");
                if (found != std::string::npos)
                {
                    std::string index = line.substr(0, found);
                    std::string left = line.substr(found + 1);

                    playersLeft[std::stoi(index)] = std::stof(left);
                }
            }

            message = "SET:OK";
        }
        else if (line == "SET_BALL")
        {
            /*while (std::getline(stream, line, '|'))
            {
                auto found = line.find("#");
                if (found != std::string::npos)
                {
                    std::string left = line.substr(0, found);
                    std::string top = line.substr(found + 1);

                    std::cout << left << " " << top << std::endl;

                    ballLeftTop = std::make_pair(std::stof(left), std::stof(top));
                }
            }*/

            line = stream.str();
            auto found = line.find("|");
            if (found != std::string::npos)
            {
                ball = line.substr(found + 1);
            }

            std::cout << ball << std::endl;

            message = "SET_BALL:OK";
        }
        else if (line == "SET_BLOCKS")
        {
            line = stream.str();
            auto found = line.find("|");
            if (found != std::string::npos)
            {
                blocks = line.substr(found + 1);
            }

            //std::cout << blocks << std::endl;

            /*while (std::getline(stream, line, '|'))
            {
                std::string index;
                std::string level;
                std::string left;
                std::string top;

                auto found1 = line.find("$");
                auto found2 = line.find("@");
                auto found3 = line.find("#");

                if (found1 != std::string::npos)
                {
                    index = line.substr(0, found1);
                }

                if (found2 != std::string::npos)
                {
                    level = line.substr(found1 + 1, found2 - found1 - 1);
                }

                if (found3 != std::string::npos)
                {
                    left = line.substr(found2 + 1, found3 - found2 - 1);
                    top = line.substr(found3 + 1);
                }

                std::cout << line << std::endl;
                std::cout << index << " " << level << " " << left << " " << top << std::endl;
            }*/

            message = "BLOCKS:OK";
        }
        else if (line == "GET")
        {
            for (auto pair : playersLeft)
            {
                message += std::to_string(pair.first) + "#" + std::to_string(pair.second) + "|";
            }
        }
        else if (line == "GET_BLOCKS")
        {
            message += blocks;
        }
        else if (line == "GET_BALL")
        {
            message += ball;
        }

        /*
        auto found = message.find("#");
        if (found != std::string::npos)
        {
            std::string index = message.substr(0, found);
            std::string left = message.substr(found + 1);

            std::cout << index << " " << left << std::endl;

            playersLeft[std::stoi(index)] = std::stof(left);
        }
        */

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
