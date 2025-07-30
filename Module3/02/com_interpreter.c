#include "com_interpreter.h"
#include <stdio.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>

static char progs_dir[PATH_MAX];

void init_progs_dir()
{
    if (readlink("/proc/self/exe", progs_dir, sizeof(progs_dir)) == -1) 
    {
        strcpy(progs_dir, ".");
    } else 
    {
        char* last_slash = strrchr(progs_dir, '/');
        if (last_slash)
        {
            *last_slash = '\0';
        }
    }
}

char* get_username()
{
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    
    char* result;

    if (pw != NULL)
    {
        result = (char*)malloc(strlen(pw->pw_name) + 1);
        return strcpy(result, pw->pw_name);
    }
    else
    {
        result = (char*)malloc(strlen("unknown") + 1);
        return strcpy(result, "unknown");
    }
}

char* get_hostname()
{
    size_t buf_size = 256;
    char* result = (char*)malloc(buf_size);

    if (gethostname(result, buf_size) == 0)
    {
        return result;
    }
    else 
    {
        return strcpy(result, "unknown");
    }
}

char* get_current_dir()
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
        result = (char*)malloc(strlen("(unknown dir)") + 1);
        return strcpy(result, "(unknown dir)");
    }
}

int execute_internal(Instruction* instr)
{
    if (strcmp(instr->argv[0], "cd") == 0)
    {
        if (instr->argc < 2)
        {
            return 1;
        }
        if (chdir(instr->argv[1]) != 0)
        {
            return 1;
        }

        return 0;
    }
    else if (strcmp(instr->argv[0], "exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }

    return -1;
}

static void execute_local(Instruction* instrs)
{
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", progs_dir, instrs->argv[0]);

    if (access(path, X_OK) == 0)
    {
        execv(path, instrs->argv);
        perror("execv error");
        _exit(EXIT_FAILURE);
    }
}

void execute(Instruction instrs[], int count)
{
    int prev_pipe = -1;
    int pipefd[2];
    pid_t pid;

    for (int i = 0; i < count; i++)
    {
        if (i < count - 1)
        {
            if (pipe(pipefd) == -1)
            {
                exit(EXIT_FAILURE);
            }
        }

        switch (pid = fork()) 
        {
            case -1:
                perror("fork error");
                exit(EXIT_FAILURE);
            
            case 0:
                if (prev_pipe != -1) 
                {
                dup2(prev_pipe, STDIN_FILENO);
                close(prev_pipe);
                }

                if (i < count - 1) 
                {
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[0]);
                    close(pipefd[1]);
                }

                execute_local(&instrs[i]);
                execvp(instrs[i].argv[0], instrs[i].argv);
                perror("execvp error");
                _exit(EXIT_FAILURE);
            
            default:
                if (prev_pipe != -1)
                {
                    close(prev_pipe);
                }

                if (i < count - 1) {
                    close(pipefd[1]);
                    prev_pipe = pipefd[0];
                }
        }
    }

    for (int i = 0; i < count; i++) 
    {
        wait(NULL);
    }
}
