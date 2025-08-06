#include "consumer.h"
#include "p_manager.h"
#include <limits.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

void consume(prod_mem* pm)
{
    while (1) 
    {
        sem_wait(pm->sem);
        
        if (pm->digits[COUNT_ID] > 0 && pm->digits[MAX_ID] == INT_MIN)
        {
            pm->digits[MAX_ID] = pm->digits[DIGITS_START];
            pm->digits[MIN_ID] = pm->digits[DIGITS_START];

            for (int i = (DIGITS_START + 1); i < pm->digits[COUNT_ID] + DIGITS_START; i++)
            {
                pm->digits[MAX_ID] = pm->digits[i] > pm->digits[MAX_ID] ? pm->digits[i] : pm->digits[MAX_ID];
                pm->digits[MIN_ID] = pm->digits[i] < pm->digits[MIN_ID] ? pm->digits[i] : pm->digits[MIN_ID];
            }
        }

        sem_post(pm->sem);
        usleep(USLEEP_DURATION);
    }
}