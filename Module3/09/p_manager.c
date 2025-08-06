#include "p_manager.h"
#include "producer.h"
#include "consumer.h"
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CONSUMERS 10

static char exec_dir[PATH_MAX];
static int childs_count = 0;
static int argc = 0;

static prod_file* prod_files;
static pid_t* childrens;

static void init_exec_dir()
{
    if (readlink("/proc/self/exe", exec_dir, sizeof(exec_dir)) == -1) 
    {
        strcpy(exec_dir, ".");
    } 
    else 
    {
        char* last_slash = strrchr(exec_dir, '/');
        if (last_slash)
        {
            *last_slash = '\0';
        }
    }
}

static int is_existing_file(char* filepath)
{
    int fd;
    struct stat f_info;

    if (stat(filepath, &f_info) == - 1)
    {
        return -1;
    }

    return 1;
}

static int is_correct_files(int args_count, char* args[])
{
    init_exec_dir();

    for (int i = 0; i < args_count; i+=2)
    {
        if (!is_existing_file(args[i]))
        {
            printf("Не удалось найти файл %s!\n", args[i]);
            return -1;
        }

        int consumers = atoi(args[i + 1]);
        if (consumers < 1 || consumers > MAX_CONSUMERS)
        {
            printf("Число потребителей должно быть в диапозоне от 1 до %d!\n", MAX_CONSUMERS);
            return -1;
        }
    }

    return 1;
}

static void terminate_childs(int childs_count, pid_t* childs, prod_file* prod_files)
{
    for (int i = 0; i < childs_count; i++)
    {
        if (childs[i] > 0)
        {
            kill(childs[i], SIGTERM);
        }
    }

    for (int i = 0; i < argc / 2; i++)
    {
        if (prod_files[i].fd != -1)
        {
            close(prod_files[i].fd);
        }
    }
}

static void check_childs(int childs_count, pid_t* childs, prod_file* prod_files)
{
    for (int i = 0; i < childs_count; i++)
    {
        if (childs[i] == -1)
        {
            perror("process creating error");
            terminate_childs(childs_count, childs, prod_files);
            exit(EXIT_FAILURE);
        }
    }
}

static int get_consumers_count(int args_count, char* args[])
{
    int counter = 0;
    for (int i = 1; i < args_count; i+=2)
    {
        counter += atoi(args[i]);
    }

    return counter;
}

static void terminate_prog(int sig)
{
    terminate_childs(childs_count, childrens, prod_files);

    for (int i = 0; i < argc / 2; i++) 
    {
        if (prod_files[i].sem)
        {
            sem_close(prod_files[i].sem);
            char sem_name[NAME_MAX];
            snprintf(sem_name, NAME_MAX, "/%s_sem", prod_files[i].filename);
            sem_unlink(sem_name);
        }
    }

    if (childrens)
    {
        free(childrens);
        childrens = NULL;
    }
    
    if (prod_files)
    {
        free(prod_files);
        prod_files = NULL;
    }

    exit(EXIT_SUCCESS);
}

void start_processing(const int args_count, char** args)
{
    argc = args_count;

    if (!is_correct_files(args_count, args))
    {
        perror("file exist error");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, terminate_prog);
    signal(SIGTERM, terminate_prog);

    prod_files = malloc(sizeof(prod_file) * (args_count / 2));
    childrens = malloc(sizeof(pid_t) * ((args_count / 2) + get_consumers_count(args_count, args)));
    for (int i = 0; i < args_count; i+=2)
    {
        childrens[childs_count] = 0;
        strcpy(prod_files[i / 2].filename, args[i]);

        char sem_name[NAME_MAX];
        snprintf(sem_name, NAME_MAX, "/%s_sem", args[i]);
        sem_t* sem = sem_open(sem_name, O_CREAT, 0666, 1);
        if (sem == SEM_FAILED) 
        {
            perror("sem_open error");
            terminate_prog(SIGTERM);
            _exit(EXIT_FAILURE);
        }

        prod_files[i / 2].sem = sem;

        int fd = open(prod_files[i / 2].filename, O_WRONLY | O_TRUNC, 0666);
        if (fd == -1)
        {
            perror("open producer file error");
            terminate_childs(childs_count,childrens, prod_files);
            terminate_prog(SIGTERM);
            _exit(EXIT_FAILURE);
        }

        prod_files[i / 2].fd = fd;

        if ((childrens[childs_count] = fork()) == 0)
        {
            produce(&prod_files[i / 2]);
            _exit(EXIT_SUCCESS);
        }
        else if (childrens[childs_count] == -1)
        {
            perror("producer create error");
            terminate_childs(childs_count,childrens, prod_files);
            terminate_prog(SIGTERM);
            _exit(EXIT_FAILURE);
        }
        childs_count++;

        fd = open(prod_files[i / 2].filename, O_RDONLY, 0666);
        if (fd == -1)
        {
            perror("open producer file error");
            terminate_childs(childs_count,childrens, prod_files);
            terminate_prog(SIGTERM);
            _exit(EXIT_FAILURE);
        }
    
        prod_files[i / 2].fd = fd;
        for (int j = 0; j < atoi(args[i + 1]); j++)
        {
            if ((childrens[childs_count] = fork()) == 0)
            {
                consume(&prod_files[i / 2]);
                _exit(EXIT_SUCCESS);
            }
            else if (childrens[childs_count] == -1)
            {
                perror("consumer create error");
                terminate_childs(childs_count,childrens, prod_files);
                terminate_prog(SIGTERM);
                _exit(EXIT_FAILURE);
            }
            childs_count++;
        }
    }

    check_childs(childs_count, childrens, prod_files);

    while (wait(NULL) > 0);
}
