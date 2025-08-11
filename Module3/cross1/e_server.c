#include "client_list.h"
#include "packets.h"
#include <arpa/inet.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>

#define SERVER_ADDR "127.0.0.1"
#define MAX_DIGITS_IN_COUNTER 20

static int sockfd;
static char buf[BUF_SIZE];

void modify_send(struct iphdr*, struct udphdr*, int);

void terminate(int sig)
{
    free_tree(get_root());

    close(sockfd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("использование: <исполняемый файл> <прослушиваемый порт>\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    if (port < 49152 || port > 65535) 
    {
        printf("Введите корректный порт сервера (> 49152 & < 65535)\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in raw_addr;
    raw_addr.sin_family = AF_INET;
    raw_addr.sin_port = htons(port);
    raw_addr.sin_addr.s_addr = INADDR_ANY;
    if (inet_aton(SERVER_ADDR, &raw_addr.sin_addr) == 0)
    {
        perror("inet_aton error");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) == -1)
    {
        perror("setsockopt error");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);

    printf("Сервер успшено запущен. Ожидание сообщений...\n");
    while (1) 
    {
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int pack_size = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*)&cli_addr, &clilen);
        if (pack_size == -1)
        {
            perror("recvfrom error");
            terminate(SIGINT);
        }

        struct iphdr* ip = (struct iphdr*) buf;
        struct udphdr* udp = (struct udphdr*)(buf + ip->ihl * 4);

        if (ntohs(udp->dest) != port)
        {
            continue;
        }

        modify_send(ip, udp, pack_size);
    }
}

void modify_send(struct iphdr* ip, struct udphdr* udp, int pack_size)
{
    char c1[MAX_ID_LEN];
    struct in_addr cli_ip;
    cli_ip.s_addr = ip->saddr;
    snprintf(c1, MAX_ID_LEN, "%s:%d", inet_ntoa(cli_ip), ntohs(udp->source));
    
    char* data = (char*)(udp + 1);
    int data_len = ntohs(udp->len) - sizeof(struct udphdr);
 
    int is_connect = 0, is_disconnect = 0;
    if (data_len == strlen("-connect") && memcmp(data, "-connect", data_len) == 0)
    {
        is_connect = 1;
    }
    else if (data_len == strlen("-disconnect") && memcmp(data, "-disconnect", data_len) == 0)
    {
        printf("Клиент %s отключился от сервера\n", c1);
        is_disconnect = 1;
        remove_client(c1);
    }

    ClientInfo *current_client;
    current_client = get_client(c1);
    if (!is_disconnect && current_client == NULL)
    {
        ClientInfo new_client;
        memset(&new_client, 0, sizeof(new_client));
        new_client.cli_addr.sin_family = AF_INET;
        new_client.cli_addr.sin_addr = cli_ip;
        new_client.cli_addr.sin_port = udp->source;
        new_client.msg_count = 0;

        if (!add_client(&new_client))
        {
            perror("add_client error");
            terminate(SIGINT);
        }
        else 
        {
            current_client = get_client(c1);
            printf("Новый клиент подключился: %s\n", c1);   
        }
    }

    if (is_connect || is_disconnect)
    {
        const char* com_msg = is_connect ? "Подключение прошло успешно" : "Отключение от сервера...";
        data_len = strlen(com_msg);
        memcpy(data, com_msg, data_len);

        udp->len = htons(sizeof(struct udphdr) + data_len);
    }
    else if (data_len + MAX_DIGITS_IN_COUNTER < BUF_SIZE - sizeof(struct iphdr) - sizeof(struct udphdr))
    {
        int bytes = snprintf(data + data_len, MAX_DIGITS_IN_COUNTER, " %d", ++current_client->msg_count);
        data_len += bytes;
        udp->len = htons(sizeof(struct udphdr) + data_len);
    }
    else
    {
        return;
    }

    uint32_t src_ip = ip->saddr;
    ip->saddr = ip->daddr;
    ip->daddr = src_ip;

    uint16_t src_port = udp->source;
    udp->source = udp->dest;
    udp->dest = src_port;

    ip->tot_len = htons(sizeof(struct iphdr) + ntohs(udp->len));

    ip->check = 0;
    ip->check = get_checksum(ip, ip->ihl * 4);
    udp->check = 0;
    udp->check = get_udp_checksum(ip, udp);

    struct sockaddr_in cli_addr;
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = ip->daddr;

    if (sendto(sockfd, buf, ntohs(ip->tot_len), 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) == -1)
    {
        perror("sendto error");
        terminate(SIGINT);
    }
}
