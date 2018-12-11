#include "wrapsock.h"

#define	bzero(ptr,n) memset(ptr, 0, n)
#define	SERV_PORT 9877 /* TCP and UDP */
#define	SA struct sockaddr
#define	MAXLINE 4096 /* max text line length */

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int			n;
    socklen_t	len;
    char		mesg[MAXLINE];

    for (; ; ) {
        len = clilen;
        n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

        Sendto(sockfd, mesg, n, 0, pcliaddr, len);
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
