#include "com_parser.h"
#include <string.h>
#include <stdio.h>

void parse(char* input, Instruction instrs[], int* instrs_count)
{
    char* saveptr1;
    char* saveptr2;
    char* tokens;
    *instrs_count = 0;
    
    tokens = strtok_r(input, "|", &saveptr1);
    while (tokens != NULL && *instrs_count < MAX_INSTRUCTIONS) 
    {
        while (*tokens == ' ') 
        {
            tokens++;
        }

        char* token_end = tokens + strlen(tokens) - 1;

        while(*token_end == ' ')
        {
            if (token_end <= tokens)
                break;
            token_end--;
        }

        *(token_end + 1) = '\0';

        instrs[*instrs_count].argc = 0;
        char *arg = strtok_r(tokens, " ", &saveptr2);
        while (arg != NULL && instrs[*instrs_count].argc < (MAX_ARGS - 1)) 
        {
            instrs[*instrs_count].argv[instrs[*instrs_count].argc++] = arg;
            arg = strtok_r(NULL, " ", &saveptr2);
        }

        instrs[*instrs_count].argv[instrs[*instrs_count].argc] = NULL;
        (*instrs_count)++;
        tokens = strtok_r(NULL, "|", &saveptr1);
    }
}
