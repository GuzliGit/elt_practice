#ifndef P_MANAGER_H
#define P_MANAGER_H

#include <linux/limits.h>
#include <sys/types.h>

typedef struct prod_file
{
    int fd;
    int semid;
    char filename[PATH_MAX];
} prod_file;

static void init_exec_dir();

static int is_existing_file(char*);

static int is_correct_files(int, char**);

static void terminate_childs(int, pid_t*, prod_file*);

static void check_childs(int, pid_t*, prod_file*);

static int get_consumers_count(int, char**);

static void terminate_prog(int);

void start_processing(const int, char**);

#endif