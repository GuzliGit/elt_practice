#ifndef DRIVER_H
#define DRIVER_H

#include <sys/types.h>

#define BUF_SIZE 1024

typedef enum DriverStatus
{
    AVAILABLE,
    BUSY
} DriverStatus;

typedef struct Driver
{
    pid_t pid;
    int write_pipefd[2];
    int read_pipefd[2];
    DriverStatus status;
    int remaining_time;
} Driver;

void start_driver(int, int);

#endif