#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_ADDRESS "127.0.0.1"
#define PORT 52666
#define BUF_SIZE 1024

static int sfd;
static char input[BUF_SIZE];
static struct sockaddr_in serv_addr;

void *receive_messages(void *arg) 
{
    char buf[BUF_SIZE];
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    
    while (1) 
    {
        ssize_t msg_len = recvfrom(sfd, buf, BUF_SIZE, 0,(struct sockaddr*)&from_addr, &addr_len);
        if (msg_len == -1) 
        {
            perror("recvfrom error");
            continue;
        }
        buf[msg_len] = '\0';
        
        if (msg_len > 0)
        {
            printf("\n[Sender]>> %s\n ", buf);
            printf("\n[You]>> ");
            fflush(stdout);
        }
    }
    return NULL;
}

static void terminate_client(int sig)
{
    sendto(sfd, "-DISCONNECT", 12, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    close(sfd);
    printf("\nВы были отключены от сервера...\n");
    exit(EXIT_SUCCESS);
}

int main()
{
    sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sfd == -1)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    
    signal(SIGINT, terminate_client);
    signal(SIGTERM, terminate_client);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); 
    serv_addr.sin_addr.s_addr = INADDR_LOOPBACK;
    inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr);

    sendto(sfd, "", 0, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    pthread_t recv_t;
    pthread_create(&recv_t, NULL, receive_messages, NULL);

    while (1) {
        printf("\n[You]>> ");
        fgets(input, BUF_SIZE, stdin);
        
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') 
        {
            input[len-1] = '\0';
        }
        
        if (strcmp(input, "-DISCONNECT") == 0)
        {
            terminate_client(SIGTERM);
        }
        
        sendto(sfd, input, strlen(input), 0,(struct sockaddr*)&serv_addr, sizeof(serv_addr));
        
        fflush(stdout);
    }
}