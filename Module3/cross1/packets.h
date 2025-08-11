#ifndef PACKETS_H
#define PACKETS_H

#include <stddef.h>
#include <stdint.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define BUF_SIZE 65536

uint16_t get_checksum(void*, size_t);

uint16_t get_udp_checksum(struct iphdr*, struct udphdr*);

#endif