#include "p_manager.h"
#include "producer.h"
#include "consumer.h"
#include <fcntl.h>
#include <limits.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>

#define SHM_NAME "/shared_mem11"

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
        snprintf(sem_name, NAME_MAX, "/%d_sem", sh_mem.shm_fd);
        sem_unlink(sem_name);
    }
    if (sh_mem.digits)
    {
        munmap(sh_mem.digits, sh_mem.buf_size);
    }
    if (sh_mem.shm_fd != -1)
    {
        close(sh_mem.shm_fd);
        shm_unlink(SHM_NAME);
    }

    exit(EXIT_SUCCESS);
}

void start_processing()
{
    processed_count = 0;

    sh_mem.shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (sh_mem.shm_fd == -1)
    {
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(sh_mem.shm_fd, SHARED_MEM_SIZE) == -1)
    {
        perror("ftruncate error");
        close(sh_mem.shm_fd);
        exit(EXIT_FAILURE);
    }

    int* shm_ptr = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sh_mem.shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        perror("mmap error");
        close(sh_mem.shm_fd);
        exit(EXIT_FAILURE);
    }

    char sem_name[NAME_MAX];
    snprintf(sem_name, NAME_MAX, "/%d_sem", sh_mem.shm_fd);
    sem_t* sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) 
    {
        perror("sem_open error");
        release_data(SIGTERM);
        exit(EXIT_FAILURE);
    }
    
    sh_mem.digits = shm_ptr;
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
