#ifndef CALC_H
#define CALC_H

typedef struct Operation
{
    char op;
    double (*func)(double, double);
    int is_unary;
} Operation;

double sum(double, double);

double substraction(double, double);

double multiply(double, double);

double divide(double, double);

double un_minus(double, double);

const Operation* get_op(char op);

int is_valid_num(const char*, double*);

double calculate(char*, char*);

#endif