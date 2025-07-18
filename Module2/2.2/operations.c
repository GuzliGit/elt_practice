#include "operations.h"
#include <stdarg.h>

double sum(int args_count, ...)
{
    va_list args;
    va_start(args, args_count);
    
    double total = 0.0;
    
    for (int i = 0; i < args_count; i++) {
        total += va_arg(args, double);
    }
    
    va_end(args);
    
    return total;
}

double substraction(double x, double y)
{
    return x - y;
}

double multiplication(double x, double y)
{
    return x * y;
}

int division(double x, double y, double *result)
{
    if (y == 0.0)
        return 0;

    *result = x / y; 
    return 1;
}
