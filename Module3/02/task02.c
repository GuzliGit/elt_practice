#include "interpreter_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 1024

int main()
{
    char input[MAX_INPUT];
    prepare_interpreter();
    
    while (1) 
    {
        print_invite();
        fflush(stdout);

        if (fgets(input, MAX_INPUT, stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        run(input);
    }
    printf("\n");

    return EXIT_SUCCESS;
}
