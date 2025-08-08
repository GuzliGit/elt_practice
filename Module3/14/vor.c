#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define BIN_FILE "bin_output.txt"
#define DEC_FILE "output.txt"
#define BUF_SIZE 65536

static char buf[BUF_SIZE];
static int sockfd, binfd, decodedfd;
static int vor_counter = 0;

void process_pack(int size);

void terminate(int sig)
{
    close(binfd);
    close(sockfd);
    exit(EXIT_SUCCESS);
}

void print_welcome()
{
    printf(" _   _     _       __ \n");
    printf("| | | |   (_)     / _|\n");
    printf("| |_| |__  _  ___| |_ \n");
    printf("| __| '_ \\| |/ _ \\  _|\n");
    printf("| |_| | | | |  __/ |  \n");
    printf(" \\__|_| |_|_|\\___|_|  \n");
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("использование: <исполняемый файл> <IPv4 сервера>\n");
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
    if (inet_aton(argv[1], &raw_addr.sin_addr) == 0)
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

    binfd = open(BIN_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    decodedfd = open(DEC_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (binfd == -1 || decodedfd == -1)
    {
        perror("open file error");
        exit(EXIT_FAILURE);
    }

    print_welcome();
    while (1) 
    {
        unsigned int rawlen = sizeof(raw_addr);
        int pack_size = recvfrom(sockfd, &buf[0], BUF_SIZE, 0, (struct sockaddr*)&raw_addr, &rawlen);
        if (pack_size == -1)
        {
            perror("recv error");
            exit(EXIT_FAILURE);
        }
        else if (pack_size == 0)
        {
            continue;
        }

        process_pack(pack_size);
    }
}

int decode_write()
{
    char output[BUF_SIZE] = {0};

    struct iphdr* ip_header = (struct iphdr*)buf;
    unsigned int ip_header_len = ip_header->ihl * 4;

    struct udphdr* udp_header = (struct udphdr*)(buf + ip_header_len);

    char* data = (buf + ip_header_len + sizeof(struct udphdr));
    int data_len = ntohs(udp_header->len) - sizeof(struct udphdr);

    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(ip_header->saddr), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip_header->daddr), dst_ip, INET_ADDRSTRLEN);

    uint16_t dest = ntohs(udp_header->dest);
    if (dest < 49152)
    {
        return 0;
    }

    int offset = snprintf(output, BUF_SIZE, "|| %s==>%s ||\n", src_ip, dst_ip);
    offset += snprintf(output + offset, BUF_SIZE - offset, "PORTS: %u==>%u\n", ntohs(udp_header->source), dest);
    offset += snprintf(output + offset, BUF_SIZE - offset, "LEN: %u\n", ntohs(udp_header->len));
    offset += snprintf(output + offset, BUF_SIZE - offset, "CHECKSUM: 0x%04X\n", ntohs(udp_header->check));
    offset += snprintf(output + offset, BUF_SIZE - offset, "DATA LEN: %d\n", data_len);
    offset += snprintf(output + offset, BUF_SIZE - offset, "MSG: ");
    for (int i = 0; i < data_len; i++) 
    {
        if (data[i] >= 32 && data[i] <= 126)
        {
            offset += snprintf(output + offset, BUF_SIZE - offset, "%c", data[i]);
        }
        else
            offset += snprintf(output + offset, BUF_SIZE - offset, "_");
    }

    offset += snprintf(output + offset, BUF_SIZE - offset, "\n");
    
    if (write(decodedfd, output, strlen(output)) == -1)
    {
        close(decodedfd);
        close(binfd);
        close(sockfd);
        perror("write to decoded_file error");
        exit(EXIT_FAILURE);
    }

    return 1;
}

void process_pack(int size)
{
    if (!decode_write())
    {
        return;
    }

    if (write(binfd, buf, size) == -1)
    {
        close(decodedfd);
        close(binfd);
        close(sockfd);
        perror("write to bin_file error");
        exit(EXIT_FAILURE);
    }

    vor_counter++;
    printf("A chto u nas v karmanah? x%d\n", vor_counter);
}