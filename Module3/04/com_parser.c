#include "com_parser.h"
#include <string.h>
#include <stdio.h>

void capture_redirect(char* arg, int* is_input, int* is_output, int* is_append)
{
    if (strcmp(arg, ">") == 0)
    {
        *is_output = 1;
    }
    else if (strcmp(arg, ">>") == 0)
    {
        *is_output = 1;
        *is_append = 1;
    }
    else if (strcmp(arg, "<") == 0)
    {
        *is_input = 1;
    }
}

int parse(char* input, Instruction instrs[], int* instrs_count)
{
    char* saveptr1;
    char* saveptr2;
    char* tokens;
    int is_input = 0, is_output = 0, is_append = 0;
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
            capture_redirect(arg, &is_input, &is_output, &is_append);

            if (is_output)
            {
                arg = strtok_r(NULL, " ", &saveptr2);
                if (arg == NULL)
                {
                    printf("Not enough arguments!\n");
                    return 0;
                }
                instrs[*instrs_count].output_file = arg;
                instrs[*instrs_count].is_append = is_append;
                is_output = 0;
                is_append = 0;
            }
            else if (is_input)
            {
                arg = strtok_r(NULL, " ", &saveptr2);
                if (arg == NULL)
                {
                    printf("Not enough arguments!\n");
                    return 0;
                }
                instrs[*instrs_count].input_file = arg;
                is_input = 0;
            }
            else
            {
                instrs[*instrs_count].argv[instrs[*instrs_count].argc++] = arg;
            }
            arg = strtok_r(NULL, " ", &saveptr2);
        }

        instrs[*instrs_count].argv[instrs[*instrs_count].argc] = NULL;
        (*instrs_count)++;
        tokens = strtok_r(NULL, "|", &saveptr1);
    }

    return 1;
}
