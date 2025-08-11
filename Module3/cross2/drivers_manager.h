#ifndef DRIVERS_MANAGER_H
#define DRIVERS_MANAGER_H

#include <sys/types.h>

#define MAX_INPUT 1024
#define MAX_RESPONSE 1024
#define MAX_DRIVERS 32
#define MAX_COM_LEN 128
#define MAX_ARGS 16

typedef struct Command
{
    char command[MAX_COM_LEN];
    int args_count;
    int args[MAX_ARGS];
} Command;

static void init_manager();

static int parse_input(char*);

static void create_driver();

static void send_task(pid_t, int);

static void get_status(pid_t);

static void remove_driver(pid_t);

static void get_drivers();

static int execute_command(char*);

static void sigchld_handler(int);

void run_cli();

#endif