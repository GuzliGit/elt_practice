#include "producer.h"
#include "p_manager.h"
#include <stdio.h>
#include <limits.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>

void produce(prod_mem* pm, int* processed_counter)
{
    srand(time(NULL) ^ getpid());

    while (1) 
    {
        int digits_count = rand() % MAX_DIGITS + 1;

        sem_wait(pm->sem);

        pm->digits[COUNT_ID] = digits_count > MAX_DIGITS ? MAX_DIGITS : digits_count;
        pm->digits[MAX_ID] = INT_MIN;
        pm->digits[MIN_ID] = INT_MAX;

        printf("{SHARED MEMORY}: ");
        for (int i = DIGITS_START; i < (pm->digits[COUNT_ID] + DIGITS_START); i++)
        {
            pm->digits[i] = rand() % MAX_NUM;
            printf("[%d] ", pm->digits[i]);
        }
        printf("\n");

        sem_post(pm->sem);

        while (1) 
        {
            sem_wait(pm->sem);

            if (pm->digits[MAX_ID] != INT_MIN && pm->digits[MIN_ID] != INT_MAX)
            {
                printf("[%d]: COUNT: %d| MAX: %d| MIN: %d\n", *processed_counter, pm->digits[COUNT_ID], pm->digits[MAX_ID], pm->digits[MIN_ID]);
                (*processed_counter)++;
                sem_post(pm->sem);
                break;
            }

            sem_post(pm->sem);
            usleep(USLEEP_DURATION);
        }

        sleep(1);
    }
}
