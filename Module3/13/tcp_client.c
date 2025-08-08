#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define BUF_SIZE 1024

static int my_sock;

void error(char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void terminate(int sig)
{
    close(my_sock);
    exit(EXIT_SUCCESS);
}

void send_file(int sock, const char* filename) 
{
    struct stat file_stat;
    if (stat(filename, &file_stat) < 0) 
    {
        printf("ERROR could not get file size for %s\n", filename);
        return;
    }
    long filesize = file_stat.st_size;

    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) 
    {
        printf("ERROR could not open file %s: %s\n", filename, strerror(errno));
        return;
    }

    char command[BUF_SIZE];
    snprintf(command, sizeof(command), "upload %s\n", filename);
    if (send(sock, command, strlen(command), 0) < 0) 
    {
        perror("send upload command");
        close(file_fd);
        return;
    }

    char ready[7];
    if (recv(sock, ready, 7, 0) < 3) 
    {
        printf("Unknown upload\n");
        close(file_fd);
        return;
    }
    ready[6] = '\0';

    if (strcmp(ready, "READY") != 0) 
    {
        printf("server refuse upload\n");
        close(file_fd);
        return;
    }

    char size_str[64];
    snprintf(size_str, sizeof(size_str), "%ld", filesize);
    if (send(sock, size_str, strlen(size_str), 0) < 0) 
    {
        perror("send file size");
        close(file_fd);
        return;
    }

    char buffer[BUF_SIZE];
    ssize_t bytes_read;
    size_t total_bytes = 0;
    
    printf("Uploading %s...\n", filename);
    
    while ((bytes_read = read(file_fd, buffer, BUF_SIZE)) > 0) 
    {
        if (send(sock, buffer, bytes_read, 0) < 0) 
        {
            perror("send file data");
            break;
        }

        total_bytes += bytes_read;
        printf("\rSent: %zu bytes", total_bytes);
        fflush(stdout);
    }

    printf("\n");
    close(file_fd);
}

int main(int argc, char* argv[])
{
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buff[BUF_SIZE];

    printf("TCP DEMO CLIENT\n");

    if (argc < 3) 
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);

    portno = atoi(argv[2]);
    my_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (my_sock < 0)
    {
        error("ERROR opening socket\n");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(my_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR connectiong\n");
    }

    while ((n = recv(my_sock, &buff[0], sizeof(buff), 0)) > 0)
    {
        buff[n] = 0;

        printf("S=>C:%s\n", buff);

        printf("S<=C:");
        fgets(&buff[0], sizeof(buff) - 1, stdin);

        if (strncmp(buff, "upload ", 7) == 0) 
        {
            char filename[PATH_MAX];
            strncpy(filename, buff + 7, BUF_SIZE);
            filename[strcspn(filename, "\n")] = '\0';
            
            send_file(my_sock, filename);
            continue;
        }

        if (!strcmp(&buff[0], "quit\n")) 
        {
            printf("Exit...\n");
            close(my_sock);
            return EXIT_SUCCESS;
        }

        send(my_sock, &buff[0], strlen(&buff[0]), 0);
    }

    printf("Recv error \n");
    close(my_sock);
    return EXIT_FAILURE;
}