#include "arg_converter.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        printf("Укажите хотя бы один аргумент при запуске!\n");
        return EXIT_SUCCESS;
    }

    pid_t pid;
    
    switch (pid = fork()) 
    {
        case -1:
            perror("Ошибка создания процесса");
            return EXIT_FAILURE;
        case 0:
        {
            int child_counter = 2;
            char* result;
            while (child_counter < argc) 
            {
                result = convert_arg(argv[child_counter]);
                printf("CHILD | pid:%d ppid:%d result:%s\n", getpid(), getppid(), result);
                child_counter += 2;
            }
            free(result);
            _exit(EXIT_SUCCESS);
        }
        default:
        {
            int parent_counter = 1;
            char* result;
            while (parent_counter < argc) 
            {
                result = convert_arg(argv[parent_counter]);
                printf("PARENT | pid:%d ppid:%d result:%s\n", getpid(), getppid(), result);
                parent_counter += 2;
            }
            free(result);

            int status;
            wait(&status);
            printf("Потомок завершил работу со следующим статусом: %d\n", WEXITSTATUS(status));
            exit(EXIT_SUCCESS);
        }
    }
}