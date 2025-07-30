#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *find_longest_string(int argc, char *argv[]) 
{
    if (argc < 2) 
        return NULL;

    char *longest = argv[1];
    for (int i = 2; i < argc; i++) 
    {
        if (strlen(argv[i]) > strlen(longest)) 
        {
            longest = argv[i];
        }
    }

    return longest;
}

int main(int argc, char *argv[]) 
{
    char *longest = find_longest_string(argc, argv);
    if (longest) 
    {
        printf("%s\n", longest);
    }
    return EXIT_SUCCESS;
}