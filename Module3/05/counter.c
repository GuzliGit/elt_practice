#include "counter.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include <signal.h>

static char* get_current_dir()
{
    char cwd[PATH_MAX];
    char* result;

    if (getcwd(cwd, sizeof(cwd)))
    {
        result = (char*)malloc(strlen(cwd) + 1);
        return strcpy(result, cwd);
    }
    else
    {
        result = (char*)malloc(strlen(".") + 1);
        return strcpy(result, ".");
    }
}

void write_to_file(int fd, char* info)
{
    if (write(fd, info, strlen(info)) == -1)
    {
        close(fd);
        perror("write file error");
        exit(EXIT_FAILURE);
    }
}

void listener(int sig)
{
    if (sig == 2)
    {
        sig_count++;
    }

    if (sig == 2 || sig == 3)
    {
        snprintf(msg, MSG_LEN_MAX, "Signal-%d received and processed\n", sig);
        write_to_file(fd, msg);
    }

    if (sig_count >= 3)
    {
        close(fd);
        exit(EXIT_SUCCESS);
    }
}

void start_count(char* filename)
{
    char full_path[PATH_MAX];
    char* current_dir = get_current_dir();
    snprintf(full_path, PATH_MAX, "%s/%s", current_dir, filename);
    free(current_dir);

    int counter = 0;
    fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1)
    {
        perror("open file error");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, listener);
    signal(SIGQUIT, listener);
    signal(SIGABRT, listener);
    signal(SIGKILL, listener);
    signal(SIGTERM, listener);
    signal(SIGSTOP, listener);
    signal(20, listener);

    while (1) 
    {
        snprintf(msg, MSG_LEN_MAX, "%d\n", counter);
        write_to_file(fd, msg);

        counter++;
        sleep(1);
    }
}
