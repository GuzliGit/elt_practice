#include "calc/calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define BUF_SIZE 1024

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

void child_handler(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0) 
    {
        nclients--;
        printf("-disconnect\n");
        printusers();
    }
}

void stop_handler(int sig)
{
    close(sockfd);
    printf("Server shutdown...\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    char buff[BUF_SIZE];
    int newsockfd;
    int portno;
    int pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
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

    signal(SIGCHLD, child_handler);
    signal(SIGINT, stop_handler);
    signal(SIGTERM, stop_handler);

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            error("ERROR on accept");
        } 

        nclients++;
        
        struct hostent *hst;
        hst = gethostbyaddr((char *)&cli_addr.sin_addr, 4, AF_INET);
        printf("+%s [%s] new connect!\n", 
                (hst) ? hst->h_name : "Unknown host", 
                (char*)inet_ntoa(cli_addr.sin_addr));
        printusers();
        
        pid = fork();
        if (pid < 0) 
        {
            error("ERROR on fork\n");
        }
        if (pid == 0) 
        {
            close(sockfd);
            dostuff(newsockfd);
            exit(EXIT_SUCCESS);
        }
        else 
        {
            close(newsockfd);
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}

void dostuff (int sock) {
    int bytes_recv;
    char buff[20 * BUF_SIZE];
    #define str1 "Enter arithmetic expression using (+,*,/,unary -, binary - are available, parentheses are also supported)\n Write 'quit' to disconnect\n"
    write(sock, str1, strlen(str1));

    while (1) 
    {
        bytes_recv = read(sock,&buff[0],sizeof(buff));

        if (bytes_recv < 0)
        {
            error("ERROR reading from socket\n");
        }

        int new_l = 0;
        if ((new_l = strcspn(buff, "\n")) > 0)
        {
            buff[new_l] = '\0';
        }

        if (strncmp(buff, "quit", 4) == 0) {
            break;
        }

        double res;
        char error[BUF_SIZE];
        error[0] = '\0';

        res = calculate(buff, error);
        if (strlen(error) == 0)
        {
            snprintf(buff, sizeof(buff), "%f", res);
        }
        else
        {
            snprintf(buff, sizeof(buff), "%s", error);
        }

        write(sock,&buff[0], strlen(buff));
        memset(buff, 0, sizeof(buff));
    }
    return;
}