#ifndef P_MANAGER_H
#define P_MANAGER_H

#include <linux/limits.h>
#include <stddef.h>
#include <sys/types.h>
#include <semaphore.h>

#define MAX_DIGITS 10
#define COUNT_ID 0
#define MAX_ID 1
#define MIN_ID 2
#define DIGITS_START 3
#define SHARED_MEM_SIZE (sizeof(int) * (MAX_DIGITS + DIGITS_START))
#define USLEEP_DURATION 10000

typedef struct prod_mem
{
    int shm_fd;
    int* digits;
    size_t buf_size;
    sem_t* sem;
} prod_mem;

static void release_data(int);

void start_processing();

#endif