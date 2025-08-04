#include "producer.h"
#include <limits.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>

static void generate_digits(char* buf, int digits_count)
{
    buf[0] = '\0';
    
    int offset = 0;
    for (int i = 0; i < digits_count; i++)
    {
        int dig = rand() % MAX_NUM;

        int current = snprintf(buf + offset, BUF_SIZE - offset, "%d ", dig);
        if (current < 0 || current >= BUF_SIZE - offset)
        {
            break;
        }
        offset += current;
    }

    if (offset > 0) 
    {
        buf[offset - 1] = '\n';
        buf[offset] = '\0';
    }
}

void produce(prod_file* pf)
{
    srand(time(NULL) ^ getpid());
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};

    while (1) 
    {
        char buf[BUF_SIZE];
        int digits_count = rand() % MAX_DIGITS_PER_LINE;

        generate_digits(buf, digits_count);

        semop(pf->semid, &lock, 1);

        write(pf->fd, buf, strlen(buf));

        semop(pf->semid, &unlock, 1);

        sleep(1);
    }
}
