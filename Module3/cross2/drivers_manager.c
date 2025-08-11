#include "drivers_manager.h"
#include "driver.h"
#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

static Driver drivers[MAX_DRIVERS];
static int drivers_count = 0;
static Command current_command;

static int epoll_fd;
struct epoll_event ev, events[MAX_DRIVERS];

static void init_manager()
{
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        perror("epoll_create error");
        exit(EXIT_FAILURE);
    }
}

static int parse_input(char* input)
{
    char* token;
    token = strtok(input, " ");
    if (token == NULL)
    {
        printf("Пустая команда\n");
        return 0;
    }

    current_command.command[0] = '\0';
    current_command.args_count = 0;

    char* token_end = token + strlen(token) - 1;
    
    size_t len = token_end - token + 1;
    if (len >= sizeof(current_command.command))
    {
        len = sizeof(current_command.command) - 1;
    }

    strncpy(current_command.command, token, len);
    current_command.command[len] = '\0';

    token = strtok(NULL, " ");
    while (token != NULL && current_command.args_count < MAX_ARGS) 
    {
        char *endptr;
        int arg = (int)strtol(token, &endptr, 10);

        if (*endptr != '\0') 
        {
            printf("Неверный аргумент: %s\n", token);
            return 0;
        }

        if (arg <= 0) 
        {
            printf("Неправильно задан следующий аргумент: %s\n", token);
            return 0;
        }

        current_command.args[current_command.args_count++] = arg;
        token = strtok(NULL, " ");
    }

    return 1;
}

static void create_driver()
{
    if (drivers_count >= MAX_DRIVERS)
    {
        printf("Достигнуто максимальное число водителей\n");
        return;
    }

    Driver* driver = &drivers[drivers_count];

    if (pipe(driver->write_pipefd) == -1)
    {
        perror("write pipe error");
        return;
    }

    if (pipe(driver->read_pipefd) == -1)
    {
        perror("read pipe error");
        close(driver->write_pipefd[0]);
        close(driver->write_pipefd[1]);
        return;
    }

    fcntl(driver->write_pipefd[1], F_SETFL, O_NONBLOCK);
    fcntl(driver->read_pipefd[0], F_SETFL, O_NONBLOCK);

    pid_t pid = fork();
    switch (pid) 
    {
        case -1:
            perror("fork error");
            close(driver->write_pipefd[0]);
            close(driver->write_pipefd[1]);
            close(driver->read_pipefd[0]);
            close(driver->read_pipefd[1]);
            return;

        case 0:
            close(driver->write_pipefd[1]);
            close(driver->read_pipefd[0]);
            
            start_driver(driver->write_pipefd[0], driver->read_pipefd[1]);
            
            close(driver->write_pipefd[0]);
            close(driver->read_pipefd[1]);
            _exit(EXIT_SUCCESS);
        default:
            close(driver->write_pipefd[0]);
            close(driver->read_pipefd[1]);

            ev.events = EPOLLIN;
            ev.data.fd = driver->read_pipefd[0];

            if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, driver->read_pipefd[0], &ev) == -1) 
            {
                perror("epoll_ctl error");
                close(driver->write_pipefd[1]);
                close(driver->read_pipefd[0]);
                return;
            }

            driver->pid = pid;
            driver->status = AVAILABLE;
            driver->remaining_time = 0;
            drivers_count++;
            printf("Создан новый водитель: %d\n", pid);
    }
}

static void send_task(pid_t pid, int task_time)
{
    for (int i = 0; i < drivers_count; i++) 
    {
        if (drivers[i].pid == pid) 
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "TASK %d\n", task_time);

            if (write(drivers[i].write_pipefd[1], buf, strlen(buf)) == -1) 
            {
                perror("write to driver pipe error");
            }
            return;
        }
    }
    printf("Водитель %d не найден\n", pid);
}

static void get_status(pid_t pid)
{
    for (int i = 0; i < drivers_count; i++) {
        if (drivers[i].pid == pid) {
            char* buf = "STATUS\n";

            if (write(drivers[i].write_pipefd[1], buf, strlen(buf)) == -1) 
            {
                perror("write to driver pipe error");
                return;
            }
            return;
        }
    }
    printf("Водитель %d не найден\n", pid);
}

static void remove_driver(pid_t pid)
{
    for (int i = 0; i < drivers_count; i++)
    {
        if (drivers[i].pid == pid)
        {
            char* buf = "REMOVE";

            if (write(drivers[i].write_pipefd[1], buf, strlen(buf)) == -1)
            {
                perror("write to driver pipe error");
                return;
            }

            if (i < drivers_count - 1)
            {
                drivers[i] = drivers[drivers_count - 1];
            }
            drivers_count--;
            return;
        }
    }
    printf("Водитель %d не найден\n", pid);
}

static void get_drivers()
{
    for (int i = 0; i < drivers_count; i++) 
    {
        get_status(drivers[i].pid);
    }
}

static int execute_command(char* input)
{
    if (!parse_input(input))
    {
        return 0;
    }

    if (strcmp(current_command.command, "create_driver") == 0) 
    {
        create_driver();
    } 
    else if (strcmp(current_command.command, "send_task") == 0) 
    {
        if (current_command.args_count != 2) 
        {
            printf("использование: send_task <pid> <task_timer>\n");
            return 0;
        }
        send_task(current_command.args[0], current_command.args[1]);
    }
    else if (strcmp(current_command.command, "get_status") == 0) 
    {
        if (current_command.args_count != 1) 
        {
            printf("использование: get_status <pid>\n");
            return 0;
        }
        get_status(current_command.args[0]);
    }
    else if (strcmp(current_command.command, "remove_driver") == 0)
    {
        if (current_command.args_count != 1)
        {
            printf("использование: remove_driver <pid>\n");
            return 0;
        }
        remove_driver(current_command.args[0]);
    }
    else if (strcmp(current_command.command, "get_drivers") == 0) 
    {
        get_drivers();
    }
    else 
    {
        printf("Неизвестная команда: %s\n", current_command.command);
    }

    return 1;
}

static void sigchld_handler(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void run_cli()
{
    char input[MAX_INPUT] = {0};
    init_manager();
    signal(SIGCHLD, sigchld_handler);

    printf("[taxi_bo$$]>> ");
    fflush(stdout);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1)
    {
        perror("epoll_ctl error");
        exit(EXIT_FAILURE);
    }

    while (1) 
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_DRIVERS, -1);
        if (nfds == -1)
        {
            perror("epoll_wait error");
            continue;
        }

        for (int i = 0; i < nfds; i++) 
        {
            if (events[i].data.fd == STDIN_FILENO) 
            {
                char buf[MAX_INPUT];
                ssize_t count = read(STDIN_FILENO, buf, sizeof(buf));
                if (count <= 0)
                {
                    perror("read from stdin error");
                    exit(EXIT_FAILURE);
                } 

                if (count > 0 && buf[count-1] == '\n') 
                {
                    buf[count-1] = '\0';
                }

                execute_command(buf);

                printf("[taxi_bo$$]>> ");
                fflush(stdout);
            }
            else
            {
                for (int j = 0; j < drivers_count; j++) 
                {
                    if (events[i].data.fd == drivers[j].read_pipefd[0]) 
                    {
                        char response[MAX_RESPONSE];
                        ssize_t count = read(drivers[j].read_pipefd[0], response, sizeof(response)-1);
                        response[count] = '\0';
                        printf("|| Водитель %d: %s", drivers[j].pid, response);
                        
                        if (strstr(response, "AVAILABLE"))
                        {
                            drivers[j].status = AVAILABLE;
                        }
                        else if (strstr(response, "BUSY"))
                        {
                            drivers[j].status = BUSY;
                        }
                        
                        printf("[taxi_bo$$]>> ");
                        fflush(stdout);
                    }
                }
            }
        }
    }
}
