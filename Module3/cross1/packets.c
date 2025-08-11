#include "packets.h"
#include <string.h>

uint16_t get_checksum(void* data, size_t len)
{
    uint32_t sum = 0;
    uint16_t *data_ptr = data;

    while (len > 1) 
    {
        sum += *data_ptr++;
        if (sum >> 16)
        {
            sum = (sum & 0xFFFF) + (sum >> 16);
        }
        len -= 2;
    }

    if (len)
    {
        sum += *(uint8_t*) data_ptr;
    }

    while (sum >> 16) 
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}

uint16_t get_udp_checksum(struct iphdr* ip, struct udphdr* udp)
{
    struct pseudo {
        uint32_t src;
        uint32_t dst;
        uint8_t zero;
        uint8_t protocol;
        uint16_t udp_len;
    } ps;

    ps.src = ip->saddr;
    ps.dst = ip->daddr;
    ps.zero = 0;
    ps.protocol = IPPROTO_UDP;
    ps.udp_len = udp->len;

    char temp_pack[BUF_SIZE];
    memcpy(temp_pack, &ps, sizeof(ps));
    memcpy(temp_pack + sizeof(ps), udp, ntohs(udp->len));

    return get_checksum(temp_pack, sizeof(ps) + ntohs(udp->len));
}
