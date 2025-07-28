#include "arg_converter.h"
#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static int is_digitc(char* arg)
{
    char* endptr;
    double res = strtod(arg, &endptr);

    return (endptr != arg && *endptr == '\0');
}

char* convert_arg(char* arg)
{
    size_t len;
    char* endptr;
    char* copy;
    len = strlen(arg);
    
    if (!is_digitc(arg))
    {
        copy = (char*)malloc(len + 1);
        return strcpy(copy, arg);
    }

    double res = strtod(arg, &endptr);
    res *= 2;
    
    if (res == HUGE_VAL || res == -HUGE_VAL)
    {
        char* res = "inf";
        len = strlen(res);
        copy = (char*)malloc(len + 1);
        return copy;
    }

    copy = (char*)malloc(50);
    snprintf(copy, 50, "%.15g", res);
    return copy;
}
