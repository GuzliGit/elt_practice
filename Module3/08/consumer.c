#include "consumer.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

static int get_max_int(char* buf, ssize_t* bytes)
{
    char* line_end = strchr(buf, '\n');
    if (line_end) 
    {
        *line_end = '\0';
        *bytes = line_end - buf + 1;
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
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};

    while (1) 
    {
        semop(pf->semid, &lock, 1);

        char buf[BUF_SIZE];
        ssize_t bytes = read(pf->fd, buf, sizeof(buf) - 1);
        
        if (bytes > 0) 
        {
            buf[bytes] = '\0';
            ssize_t processed_bytes = bytes;
            int max_dig = get_max_int(buf, &processed_bytes);

            if (max_dig != INT_MIN) 
            {
                off_t pos_before = lseek(pf->fd, 0, SEEK_CUR);

                lseek(pf->fd, pos_before - bytes + processed_bytes, SEEK_SET);
                
                off_t pos = lseek(pf->fd, 0, SEEK_CUR);
                printf("[%s]: Позиция: %ld, PID: %d, Макс.число: %d\n", 
                    pf->filename, pos, getpid(), max_dig);
            }
        }
        
        semop(pf->semid, &unlock, 1);
        
        sleep(2);
    }
}