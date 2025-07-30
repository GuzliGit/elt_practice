#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int sum_numbers(int argc, char *argv[]) 
{
    char buf[256];
    int sum = 0;
    for (int i = 1; i < argc; i++) 
    {
        if (isdigit(argv[i][0]) || (argv[i][0] == '-' && isdigit(argv[i][1]))) 
        {
            sum += atoi(argv[i]);
        }
    }

    while (fgets(buf, sizeof(buf), stdin)) 
    {
        sum += atoi(buf);
    }

    return sum;
}

int main(int argc, char *argv[]) 
{
    printf("%d\n", sum_numbers(argc, argv));
    return EXIT_SUCCESS;
}