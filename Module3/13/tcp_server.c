#include "calc/calc.h"
#include <fcntl.h>
#include <linux/limits.h>
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
#include <sys/stat.h>

#define BUF_SIZE 64000
#define FILES_DIR "server_files"

static int sockfd;
static int nclients = 0;

void upload_file(int, const char*);
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

void create_files_dir()
{
    struct stat st = {0};
    if (stat(FILES_DIR, &st) == -1)
    {
        mkdir(FILES_DIR, 0700);
    }
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

    create_files_dir();

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

void upload_file(int sock, const char* filename)
{
    const char *cor_name = strrchr(filename, '/');
    if (cor_name == NULL) 
    {
        cor_name = filename;
    } 
    else 
    {
        cor_name++;
    }

    char filepath[PATH_MAX];
    snprintf(filepath, sizeof(filepath), "%s/%s", FILES_DIR, cor_name);
    
    int file_fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd < 0) 
    {
        char error_msg[BUF_SIZE];
        snprintf(error_msg, sizeof(error_msg), "ERROR could not create file %s\n", cor_name);
        write(sock, error_msg, strlen(error_msg));
        return;
    }
    
    char buffer[BUF_SIZE];
    ssize_t bytes_received;
    size_t total_bytes = 0;
    
    write(sock, "READY", 5);
    
    char size_str[64] = {0};
    int n = read(sock, size_str, sizeof(size_str)-1);
    if (n <= 0) 
    {
        close(file_fd);
        return;
    }
    long filesize = atol(size_str);

    while (total_bytes < (size_t)filesize && (bytes_received = read(sock, buffer, BUF_SIZE)) > 0) 
    {
        if (bytes_received < 0) 
        {
            perror("read file");
            break;
        }
        
        ssize_t bytes_written = write(file_fd, buffer, bytes_received);
        if (bytes_written != bytes_received) 
        {
            perror("write file");
            break;
        }
        
        total_bytes += bytes_received;
    }
    
    close(file_fd);
    
    if (bytes_received < 0) 
    {
        perror("read file");
        unlink(filepath);
        return;
    }

    char success_msg[BUF_SIZE];
    snprintf(success_msg, sizeof(success_msg), "File %s uploaded successfully (%zu bytes)\n", cor_name, total_bytes);
    write(sock, success_msg, strlen(success_msg));
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

        if (strncmp(buff, "upload", strlen("upload")) == 0) 
        {
            char filename[PATH_MAX];
            char *space = strchr(buff, ' ');
            
            if (space != NULL) 
            {
                strncpy(filename, space + 1, sizeof(filename)-1);
                filename[sizeof(filename)-1] = '\0';
                
                char *newline = strchr(filename, '\n');
                if (newline) *newline = '\0';
                
                upload_file(sock, filename);
            } 
            else 
            {
                write(sock, "ERROR invalid upload command format. Use: upload <filename>\n", 58);
            }
            
            memset(buff, 0, sizeof(buff));
            continue;
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