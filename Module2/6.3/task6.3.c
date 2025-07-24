#include "calc.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define INPUT_MAX 256
#define LIBS_DIR "./operations"

void print_result(char* input) 
{
    char error[INPUT_MAX] = {0};
    double result = calculate(input, error);
    
    printf("\033[1B");
    
    if (isnan(result)) 
    {
        printf("\033[31mError: %s\033[0m\n", error);
    } 
    else 
    {
        printf("\033[32mResult: %g\033[0m\n", result);
    }
}

int main()
{
    if (!load_ops(LIBS_DIR))
    {
        printf("Incorrect lib path!\n");
        return 1;
    }
    
    char input[INPUT_MAX];
    
    printf("Calculator (Ctrl+C to exit)\n");
    printf("Input expression: ");
    fflush(stdout);
    
    while (fgets(input, INPUT_MAX, stdin)) 
    {
        input[strcspn(input, "\n")] = '\0';
        
        if (strlen(input) == 0) 
        {
            printf("\033[1A"); 
            continue;
        }
        
        print_result(input);
        
        printf("\nInput expression: ");
        fflush(stdout);
    }

    free_ops();
    return 0;
}
