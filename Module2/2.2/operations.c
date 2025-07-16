#include "operations.h"

double sum(double x, double y)
{
    return x + y;
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
