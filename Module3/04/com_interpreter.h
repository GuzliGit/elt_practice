#ifndef COM_INTERPRETER_H
#define COM_INTERPRETER_H

#define MAX_ARGS 20
#define MAX_INSTRUCTIONS 20

typedef struct Instruction 
{
    char* argv[MAX_ARGS];
    int argc;
    char* input_file;
    char* output_file;
    int is_append;
} Instruction;

void init_progs_dir();

char* get_username();

char* get_hostname();

char* get_current_dir();

int execute_internal(Instruction*);

static void execute_local(Instruction*);

void execute(Instruction[], int);

#endif