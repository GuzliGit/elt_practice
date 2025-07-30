#ifndef COM_INTERPRETER_H
#define COM_INTERPRETER_H

#define MAX_ARGS 10
#define MAX_INSTRUCTIONS 20

typedef struct Instruction 
{
    char* argv[MAX_ARGS];
    int argc;
} Instruction;

void init_progs_dir();

char* get_username();

char* get_hostname();

char* get_current_dir();

int execute_internal(Instruction*);

static void execute_local(Instruction*);

void execute(Instruction[], int);

#endif