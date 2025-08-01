#include "interpreter_api.h"
#include "com_interpreter.h"
#include "com_parser.h"
#include <stdio.h>
#include <stdlib.h>

void prepare_interpreter()
{
    init_progs_dir();
}

void print_invite()
{
    char* username;
    char* hostname;
    char* current_dir;
    username = get_username();
    hostname = get_hostname();
    current_dir = get_current_dir();


    printf("[%s@%s %s]► ", username, hostname, current_dir);

    free(username);
    free(hostname);
    free(current_dir);
}

static void init_instructions(Instruction* instrs)
{
    for (int i = 0; i < MAX_INSTRUCTIONS; i++)
    {
        instrs[i].input_file = NULL;
        instrs[i].output_file = NULL;
        instrs[i].is_append = 0;
    }
}

void run(char* input)
{
    int instrs_count = 0;
    Instruction instrs[MAX_INSTRUCTIONS];
    init_instructions(instrs);

    if (!parse(input, instrs, &instrs_count))
    {
        printf("[!] input/output redirect error!\n");
        return;
    }

    if (instrs_count == 1 && execute_internal(instrs) != -1)
    {
        return;
    }

    execute(instrs, instrs_count);
}
