#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

void invert(char *str) 
{
    while (*str) 
    {
        if (*str >= 'A' && *str <= 'Z') 
        {
            *str = tolower(*str);
        } 
        else if (*str >= 'a' && *str <= 'z') 
        {
            *str = toupper(*str);
        }
        str++;
    }
}

int main(int argc, char *argv[]) 
{
    for (int i = 1; i < argc; i++) 
    {
        invert(argv[i]);
        printf("%s ", argv[i]);
    }
    printf("\n");
    return EXIT_SUCCESS;
}