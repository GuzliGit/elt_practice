#ifndef CALC_H
#define CALC_H

#include <dlfcn.h>
#include <dirent.h>
#include <stdlib.h>

typedef struct Operation
{
    char op;
    double (*func)(double, double);
    int is_unary;
    void* handle;
} Operation;

int load_ops(char*);

void free_ops();

int reload_op(char);

void update_load_memory();

const Operation* get_op(char op);

int is_valid_num(const char*, double*);

double calculate(char*, char*);

#endif