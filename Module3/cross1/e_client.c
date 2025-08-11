#include "packets.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>

static int sockfd;
static char buf[BUF_SIZE];
static struct sockaddr_in cli_addr;

static char local_ip[INET_ADDRSTRLEN];
static char server_ip[INET_ADDRSTRLEN];
static uint16_t local_port;
static uint16_t server_port;

void send_msg(const char*);
void recv_msg();

int get_local_ip_and_port(const char* server_ip, int server_port, char* local_ip_str, size_t ip_str_len, uint16_t* local_port)
{
    int tmp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (tmp_sock == -1) 
    {
        perror("socket error");
        return -1;
    }

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    if (connect(tmp_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        perror("connect error");
        close(tmp_sock);
        return -1;
    }

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(tmp_sock, (struct sockaddr*)&local_addr, &addr_len) < 0) 
    {
        perror("getsockname error");
        close(tmp_sock);
        return -1;
    }

    inet_ntop(AF_INET, &local_addr.sin_addr, local_ip_str, ip_str_len);
    *local_port = ntohs(local_addr.sin_port);

    close(tmp_sock);
    return 0;
}

void terminate(int sig)
{
    const char* disconnect_msg = "-disconnect\0";
    send_msg(disconnect_msg);
    recv_msg();

    close(sockfd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("использование: <исполняемый файл> <IPv4 сервера> <порт сервера>\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);
    if (port < 49152 || port > 65535) 
    {
        printf("Введите корректный порт сервера (>= 49152 & <= 65535)\n");
        exit(EXIT_FAILURE);
    }

    memcpy(server_ip, argv[1], strlen(argv[1]));
    server_port = port;

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(port);
    if (inet_aton(server_ip, &cli_addr.sin_addr) == 0)
    {
        perror("inet_aton error");
        exit(EXIT_FAILURE);
    }

    if (get_local_ip_and_port(server_ip, port, local_ip, sizeof(local_ip), &local_port) != 0)
    {
        perror("local ip/port error");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1)
    {
        perror("socket error");
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

    const char* connection_msg = "-connect\0";
    send_msg(connection_msg);
    recv_msg();

    char input[BUF_SIZE - sizeof(struct iphdr) - sizeof(struct udphdr)];
    printf("Введите сообщение (Ctrl+C для выхода):\n");
    printf(">> ");
    while (fgets(input, sizeof(input), stdin) != NULL)
    {
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
        {
            input[len - 1] = '\0';
        }

        send_msg(input);
        recv_msg();
        
        if (strcmp(input, "-disconnect") == 0)
        {
            close(sockfd);
            return EXIT_SUCCESS;
        }
        printf(">> ");
        fflush(stdout);
    }

    terminate(SIGINT);
    return EXIT_SUCCESS;
}

void send_msg(const char* message)
{
    memset(buf, 0, BUF_SIZE);

    struct iphdr* ip = (struct iphdr*)buf;
    ip->ihl = 5;
    ip->protocol = IPPROTO_UDP;
    ip->saddr = inet_addr(local_ip);
    ip->daddr = inet_addr(server_ip);
    ip->version = 4;
    ip->check = 0;
    ip->tot_len = 0;
    ip->id = htons(12345);
    ip->frag_off = 0;
    ip->tos = 0;
    ip->ttl = 64;

    struct udphdr* udp = (struct udphdr*)(buf + sizeof(struct iphdr));
    udp->source = htons(local_port);
    udp->dest = htons(server_port);
    udp->len = 0;
    udp->check = 0;

    char* data = (char*)(udp + 1);
    int msg_len = strlen(message);
    memcpy(data, message, msg_len);
    
    udp->len = htons(sizeof(struct udphdr) + msg_len);
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + msg_len);

    ip->check = get_checksum(ip, sizeof(struct iphdr));
    udp->check = get_udp_checksum(ip, udp);

    if (sendto(sockfd, buf, ntohs(ip->tot_len), 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) == -1)
    {
        perror("sendto error");
        terminate(SIGINT);
    }
}

void recv_msg()
{
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    while (1)
    {
        int pack_size = recvfrom(sockfd, buf, BUF_SIZE, 0, (struct sockaddr*)&from_addr, &from_len);
        if (pack_size == -1)
        {
            perror("recvfrom error");
            return;
        }

        struct iphdr* ip = (struct iphdr*)buf;
        struct udphdr* udp = (struct udphdr*)(buf + ip->ihl * 4);

        if (ip->saddr == inet_addr(server_ip) && ntohs(udp->dest) == local_port)
        {
            int udp_data_len = ntohs(udp->len) - sizeof(struct udphdr);
            char* udp_data = (char*)(udp + 1);

            printf("SERVER>> %.*s\n", udp_data_len, udp_data);
            break;
        }
    }
}
