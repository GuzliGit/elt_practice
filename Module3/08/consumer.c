#include "consumer.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>

static int get_max_int(char* buf, ssize_t bytes)
{
    char* line_end = strchr(buf, '\n');
    if (line_end) 
    {
        *line_end = '\0';
        bytes = line_end - buf + 1;
    }
    
    int max_dig = INT_MIN;
    char* token = strtok(buf, " ");
    while (token != NULL) 
    {
        int num = atoi(token);
        if (num > max_dig) {
            max_dig = num;
        }
        token = strtok(NULL, " ");
    }

    return max_dig;
}

void consume(prod_file* pf)
{
    srand(time(NULL) ^ getpid());

    while (1) 
    {
        struct sembuf op = {0, -1, 0};
        semop(pf->semid, &op, 1);

        char buf[BUF_SIZE];
        ssize_t bytes = read(pf->fd, buf, sizeof(buf) - 1);
        
        if (bytes > 0) 
        {
            buf[bytes] = '\0';
            int max_dig = get_max_int(buf, bytes);

            if (max_dig != INT_MIN) 
            {
                off_t pos = lseek(pf->fd, 0, SEEK_CUR);
                printf("[%s]: Позиция: %ld, PID: %d, Макс.число: %d\n", 
                    pf->filename, pos, getpid(), max_dig);
            }
        }
        
        op.sem_op = 1;
        semop(pf->semid, &op, 1);
        
        sleep(2);
    }
}