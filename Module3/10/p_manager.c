#include "p_manager.h"
#include "producer.h"
#include "consumer.h"
#include <fcntl.h>
#include <limits.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>

static prod_mem sh_mem;
static pid_t child;

static int processed_count = 0;

static void release_data(int sig)
{
    printf("\nОбработано %d наборов данных\n", processed_count);

    if (child > 0)
    {
        kill(child, SIGTERM);
    }

    if (sh_mem.sem)
    {
        sem_close(sh_mem.sem);
        char sem_name[NAME_MAX];
        snprintf(sem_name, NAME_MAX, "/%d_sem", sh_mem.shmid);
        sem_unlink(sem_name);
    }
    if (sh_mem.digits)
    {
        shmdt(sh_mem.digits);
    }
    if (sh_mem.shmid != -1)
    {
        shmctl(sh_mem.shmid, IPC_RMID, NULL);
    }

    exit(EXIT_SUCCESS);
}

void start_processing()
{
    processed_count = 0;

    key_t key = ftok(IPC_PRIVATE, 'A');
    int shmid = shmget(key, SHARED_MEM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget error");
        release_data(SIGTERM);
        exit(EXIT_FAILURE);
    }

    int* shmptr = shmat(shmid, NULL, 0);
    if (shmptr == (int*)-1)
    {
        perror("shmat error");
        release_data(SIGTERM);
        exit(EXIT_FAILURE);
    }
    memset(shmptr, 0, SHARED_MEM_SIZE);

    char sem_name[NAME_MAX];
    snprintf(sem_name, NAME_MAX, "/%d_sem", shmid);
    sem_t* sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) 
    {
        perror("sem_open error");
        release_data(SIGTERM);
        exit(EXIT_FAILURE);
    }
    
    sh_mem.shmid = shmid;
    sh_mem.digits = shmptr;
    sh_mem.buf_size = SHARED_MEM_SIZE;
    sh_mem.sem = sem;
    
    switch (child = fork()) 
    {
        case -1:
            perror("process create error");
            release_data(SIGTERM);
            exit(EXIT_FAILURE);
        
        case 0:
            consume(&sh_mem);
            _exit(EXIT_SUCCESS);
        
        default:
            signal(SIGINT, release_data);
            signal(SIGTERM, release_data);

            produce(&sh_mem, &processed_count);
            wait(NULL);
    }
}
