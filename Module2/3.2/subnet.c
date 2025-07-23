#include "subnet.h"
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void ip_to_str(uint32_t ip, char* str)
{
    unsigned char octets[4];
    octets[0] = ip & 0xFF;
    octets[1] = (ip >> 8) & 0xFF;
    octets[2] = (ip >> 16) & 0xFF;
    octets[3] = (ip >> 24) & 0xFF;

    sprintf(str, "%d.%d.%d.%d", octets[3], octets[2], octets[1], octets[0]);
}

int str_to_ip(const char* str, uint32_t* ip)
{
    unsigned char octets[4];
    int count = sscanf(str, "%hhu.%hhu.%hhu.%hhu", &octets[3], &octets[2], &octets[1], &octets[0]);

    if (count != 4)
        return 0;

    *ip = (octets[3] << 24) | (octets[2] << 16) | (octets[1] << 8) | octets[0];
    return 1;
}

int start_sending_sim(char* argv[])
{
    uint32_t gateway, subnet;
    int packets = atoi(argv[2]);

    if (!str_to_ip(argv[0], &gateway))
    {
        printf("Указан некорректный IP адрес шлюза!\n");
        return 0;
    }
    if (!str_to_ip(argv[1], &subnet))
    {
        printf("Указана некорректная маска подсети!\n");
        return 0;
    }
    if (packets <= 0)
    {
        printf("Кол-во пакетов должно быть больше 0!\n");
        return 0;
    }

    srand(time(NULL));

    int in_subnet = 0, out_subnet = 0;
    for (int i = 0; i < packets; i++)
    {
        uint32_t rand_ip = ((rand() % 255) << 24) | ((rand() % 255) << 16) | ((rand() % 255) << 8) | (rand() % 255);

        if ((rand_ip & subnet) == (gateway & subnet))
        {
            in_subnet++;
        }
        else
        {
            out_subnet++;
        }
    }

    printf("%d пакетов предназначались узлам указанной подсети (%.2f%%)\n", in_subnet, 100.0 * in_subnet / packets);
    printf("%d пакетов предназначались узлам другой подсети (%.2f%%)\n", out_subnet, 100.0 * out_subnet / packets);

    return 1;
}
