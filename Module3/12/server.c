#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 52666
#define MAX_CLIENTS 2
#define BUF_SIZE 1024

static struct sockaddr_in clients[MAX_CLIENTS];
static int clients_count = 0;

static int sfd;
static char buf[BUF_SIZE];

static void try_connect(struct sockaddr_in client)
{
    for (int i = 0; i < clients_count; i++) 
    {
        if (memcmp(&clients[i], &client, sizeof(client)) == 0) {
            return;
        }
    }

    if (clients_count < MAX_CLIENTS)
    {
        memcpy(&clients[clients_count], &client, sizeof(client));
        clients_count++;
        printf("Новый клиент присоединился [%s:%d].\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        
        const char* welcome = "[Server]: Вы успешно подключены к серверу!.\n";
        sendto(sfd, welcome, strlen(welcome), 0, (struct sockaddr*)&client, sizeof(client));
    }
}

static void try_send(ssize_t msg_len, struct sockaddr_in client)
{
    for (int i = 0; i < clients_count; i++)
    {
        if (memcmp(&clients[i], &client, sizeof(client)) != 0)
        {
            sendto(sfd, buf, msg_len, 0, (struct sockaddr*)&clients[i], sizeof(clients[i]));
        }
    }
}

static void terminate_server(int sig)
{
    close(sfd);
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

    signal(SIGINT, terminate_server);
    signal(SIGTERM, terminate_server);

    const struct sockaddr_in serv_addr = {.sin_family = AF_INET, 
                                          .sin_port = htons(PORT), 
                                          .sin_addr.s_addr = INADDR_ANY};

    if ((bind(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) == -1)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен. Порт: %d.\n", PORT);
    
    while (1) 
    {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        ssize_t msg_len;
        if ((msg_len = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &addr_len)) == -1)
        {
            perror("receive error");
            continue;
        }
        buf[msg_len] = '\0';

        try_connect(client_addr);

        try_send(msg_len, client_addr);
    }
}
