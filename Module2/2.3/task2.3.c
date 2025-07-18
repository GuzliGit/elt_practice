#include "calc.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_INPUT 256

void print_result(char* input) 
{
    char error[MAX_INPUT] = {0};
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
    char input[MAX_INPUT];
    
    printf("Calculator (Ctrl+C to exit)\n");
    printf("Input expression: ");
    fflush(stdout);
    
    while (fgets(input, MAX_INPUT, stdin)) 
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

    return 0;
}
