#include "wrapsock.h"
#include "writen.h"

#define	bzero(ptr,n) memset(ptr, 0, n)
constexpr auto SERV_PORT = 9877 /* TCP and UDP */;
#define	SA struct sockaddr
constexpr auto MAXLINE = 4096 /* max text line length */;
#define pid_t int
#define	LISTENQ		1024	/* 2nd argument to listen() */

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    int         n;
    socklen_t   len;
    char        mesg[MAXLINE];

    for (; ; ) {
        len = clilen;
        n = Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        mesg[n] = '\0';
        printf("mesg: %s", mesg);
        Sendto(sockfd, mesg, n, 0, pcliaddr, len);
    }
}

void
Close(int fd)
{
    if (closesocket(fd) == -1)
        err_sys("close error");
}

void
str_echo(int sockfd)
{
    size_t		n;
    char		buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0)
        Writen(sockfd, buf, n);

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        err_sys("str_echo: read error");
}

int main(int argc, char **argv)
{
#if defined _WIN32
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
        printf("Initialization error.\n");
#endif

    int					listenfd, connfd, udpfd, nready, maxfdp1;
    char				mesg[MAXLINE];
    pid_t				childpid;
    fd_set				rset;
    size_t				n;
    socklen_t			len;
    const char			on = 1;
    struct sockaddr_in	cliaddr, servaddr;
    void				sig_chld(int);

    /* 4create listening TCP socket */
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    /* 4create UDP socket */
    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(udpfd, (SA *)&servaddr, sizeof(servaddr));
    /* end udpservselect01 */

    /* include udpservselect02 */
    //Signal(SIGCHLD, sig_chld);	/* must call waitpid() */

    FD_ZERO(&rset);
    maxfdp1 = max(listenfd, udpfd) + 1;
    for (; ; ) {
        FD_SET(listenfd, &rset);
        FD_SET(udpfd, &rset);
        if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
            if (errno == EINTR)
                continue;		/* back to for() */
            else
                err_sys("select error");
        }

        if (FD_ISSET(listenfd, &rset)) {
            len = sizeof(cliaddr);
            connfd = Accept(listenfd, (SA *)&cliaddr, &len);

            //if ((childpid = Fork()) == 0) {	/* child process */
            if (true) {
                Close(listenfd);	/* close listening socket */
                str_echo(connfd);	/* process the request */
                //exit(0);
            }
            Close(connfd);			/* parent closes connected socket */
        }

        if (FD_ISSET(udpfd, &rset)) {
            len = sizeof(cliaddr);
            n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);

            Sendto(udpfd, mesg, n, 0, (SA *)&cliaddr, len);
        }
    }

#if defined _WIN32
    WSACleanup();
#endif
}
/* end udpservselect02 */

/*
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
*/
