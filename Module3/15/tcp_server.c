#include "calc/calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <errno.h>

#define BUF_SIZE 1024
#define MAX_EVENTS 32

static struct epoll_event ev, events[MAX_EVENTS];

static int sockfd;
static int nclients = 0;

void dostuff(int);

void error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void printusers() 
{
    if(nclients) 
    {
        printf("%d user on-line\n", nclients);
    }
    else 
    {
        printf("No User on line\n");
    }
}

void stop_handler(int sig)
{
    close(sockfd);
    printf("Server shutdown...\n");
    exit(EXIT_SUCCESS);
}

void set_nonblock(int fd) 
{
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}

int main(int argc, char *argv[])
{
    char buff[BUF_SIZE];
    int newsockfd;
    int portno;
    int pid;
    struct sockaddr_in serv_addr;
    printf("TCP SERVER DEMO\n");
    
    if (argc < 2) 
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        error("ERROR opening socket\n");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding\n");
    }

    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);

    listen(sockfd, 5);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        error("ERROR epoll_create\n");
    }

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = sockfd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
    {
        error("ERROR epoll_ctl\n");
    }

    while (1)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == sockfd)
            {
                struct sockaddr_in cli_addr;
                socklen_t clilen = sizeof(cli_addr);

                int clifd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
                if (clifd < 0)
                {
                    error("ERROR on accept\n");
                }
                set_nonblock(clifd);

                nclients++;
                struct hostent *hst;
                hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
                printf("+%s [%s] new connect!\n", 
                        (hst) ? hst->h_name : "Unknown host", 
                        (char*)inet_ntoa(cli_addr.sin_addr));
                printusers();

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = clifd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clifd, &ev) == -1) 
                {
                    perror("ERROR epoll_ctl client\n");
                    close(clifd);
                    nclients--;
                    continue;
                }

                const char* hello =
                    "Enter arithmetic expression using (+,*,/, unary -, binary - are available, parentheses are also supported)\n"
                    "Write 'quit' to disconnect\n";
                send(clifd, hello, strlen(hello), 0);
            }
            else
            {
                dostuff(events[i].data.fd);
            }
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

void dostuff (int sock) {
    int bytes_recv;
    char recvbuf[BUF_SIZE];
    char sendbuf[BUF_SIZE];

    while (1) 
    {
        bytes_recv = read(sock, recvbuf, sizeof(recvbuf) - 1);
        if (bytes_recv == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            perror("read");
            close(sock);
            nclients--;
            printusers();
            return;
        }
        else if (bytes_recv == 0)
        {
            close(sock);
            nclients--;
            printf("-disconnect\n");
            printusers();
            return;
        }

        recvbuf[bytes_recv] = '\0';
        recvbuf[strcspn(recvbuf, "\r\n")] = '\0';

        if (strcmp(recvbuf, "quit") == 0)
        {
            close(sock);
            nclients--;
            printf("-disconnect\n");
            printusers();
            return;
        }

        double res;
        char error[BUF_SIZE] = {0};

        res = calculate(recvbuf, error);
        if (strlen(error) == 0)
        {
            snprintf(sendbuf, sizeof(sendbuf), "%f\n", res);
        }
        else
        {
            snprintf(sendbuf, sizeof(sendbuf), "%s\n", error);
        }

        ssize_t bytes_sent = send(sock, sendbuf, strlen(sendbuf), 0);
        if (bytes_sent == -1)
        {
            perror("send");
            close(sock);
            nclients--;
            printusers();
            return;
        }

        memset(recvbuf, 0, sizeof(recvbuf));
        memset(sendbuf, 0, sizeof(sendbuf));
    }
}