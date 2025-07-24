#include "calc.h"
#include "lexer.h"
#include "parser.h"
#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int load_memory[128] = {0};
char* path_to_libs;

Operation operations[] = 
{
    {'+', NULL, 0, NULL},
    {'-', NULL, 0, NULL},
    {'*', NULL, 0, NULL},
    {'/', NULL, 0, NULL},
    {'~', NULL, 1, NULL}
};

int load_ops(char* dir_p)
{
    DIR* dir = opendir(dir_p);
    if (!dir)
        return 0;

    struct dirent* entry;
    int loaded = 0;

    while ((entry = readdir(dir)) != NULL) 
    {
        if (strstr(entry->d_name, ".so") == NULL)
            continue;

        char op_name[256] = "\0";
        strncpy(op_name, entry->d_name, sizeof(op_name));
        op_name[strcspn(op_name, ".")] = '\0';
        char op;
        if (strcmp(op_name, "sum") == 0)
            op = '+';
        else if (strcmp(op_name, "substraction") == 0)
            op = '-';
        else if (strcmp(op_name, "multiply") == 0)
            op = '*';
        else if (strcmp(op_name, "divide") == 0)
            op = '/';
        else if (strcmp(op_name, "unary_minus") == 0)
            op = '~';
        else
            continue;

        char path[256];
        snprintf(path, sizeof(path), "%s/%s", dir_p, entry->d_name);

        void* handle = dlopen(path, RTLD_LAZY);
        if (!handle)
            continue;

        double (*func)(double, double) = dlsym(handle, "operation");
        if (!func)
        {
            dlclose(handle);
            continue;
        }

        for (int i = 0; i < sizeof(operations)/sizeof(operations[0]); i++)
        {
            if (operations[i].op == op)
            {
                operations[i].func = func;
                operations[i].handle = handle;
                load_memory[op]++;
                loaded++;
            }
        }
    }

    closedir(dir);
    path_to_libs = strdup(dir_p);
    if (loaded != sizeof(operations)/sizeof(operations[0])) 
    {
        printf("%d libs loaded, required libs count:%zu\n", loaded, sizeof(operations)/sizeof(operations[0]));
        return 0;
    }

    return 1;
}

void free_ops()
{
    for (int i = 0; i < sizeof(operations)/sizeof(operations[0]); i++) 
    {
        dlclose(operations[i].handle);
    }
}

int reload_op(char op)
{
    for (int i = 0; i < sizeof(operations)/sizeof(operations[0]); i++)
    {
        if (operations[i].op == op)
        {
            dlclose(operations[i].handle);
            operations[i].func = NULL;
            operations[i].handle = NULL;
        }
    }

    DIR* dir = opendir(path_to_libs);
    if (!dir)
        return 0;

    struct dirent* entry;
    int loaded = 0;

    while ((entry = readdir(dir)) != NULL) 
    {
        char lib_name[256];
        if (op == '+')
            strcpy(lib_name, "sum.so\0");
        else if (op == '-')
            strcpy(lib_name, "substraction.so\0");
        else if (op == '*')
            strcpy(lib_name, "multiply.so\0");
        else if (op == '/')
            strcpy(lib_name, "divide.so\0");
        else if (op == '~')
            strcpy(lib_name, "unary_minus.so\0");
        else
            continue;
        
        if (strcmp(entry->d_name, lib_name) != 0)
            continue;

        char path[256];
        snprintf(path, sizeof(path), "%s/%s", path_to_libs, entry->d_name);

        void* handle = dlopen(path, RTLD_LAZY);
        if (!handle)
            continue;

        double (*func)(double, double) = dlsym(handle, "operation");
        if (!func)
        {
            dlclose(handle);
            continue;
        }

        for (int i = 0; i < sizeof(operations)/sizeof(operations[0]); i++)
        {
            if (operations[i].op == op)
            {
                operations[i].func = func;
                operations[i].handle = handle;
                loaded++;
            }
        }
    }

    closedir(dir);

    return loaded == 1;
}

void update_load_memory()
{
    for (int i = 0; i < sizeof(operations)/sizeof(operations[0]); i++)
    {
        load_memory[operations[i].op] = 0;
    }
}

const Operation* get_op(char op)
{
    for (int i = 0; i < sizeof(operations)/sizeof(operations[0]); i++)
    {
        if (operations[i].op == op)
        {
            if (load_memory[op] == 0)
            {
                reload_op(op);
                load_memory[op]++;
            }

            return &operations[i];
        }
    }
    return NULL;
}

int is_valid_num(const char* str, double* res)
{
    char* endptr;
    *res = strtod(str, &endptr);

    if (endptr == str || *endptr != '\0')
    {
        return 0;
    }
    return 1;
}

double calculate(char* input, char* error)
{
    if (!tokenize(input))
    {
        sprintf(error, "Invalid tokens!");
        return NAN;
    }

    char rpn[MAX_RPN_LEN];
    if (!get_rpn(get_tokens_buf(), get_tokens_count(), rpn, error))
    {
        return NAN;
    }
    
    double stack[MAX_TOKENS];
    int stack_size = 0;
    char token[MAX_TOKEN_LEN];
    int token_len = 0;

    for (int i = 0; rpn[i] != '\0'; i++)
    {
        if (rpn[i] == ' ')
        {
            if (token_len > 0)
            {
                token[token_len] = '\0';

                double num;
                if (is_valid_num(token, &num))
                {
                    stack[stack_size++] = num;
                }
                else
                {
                    sprintf(error, "Invalid num: %s", token);
                    return NAN;
                }
                token_len = 0;
            }
            continue;;
        }

        if (isdigit(rpn[i]) || rpn[i] == '.') 
        {
            token[token_len++] = rpn[i];
            continue;
        }

        const Operation* op = get_op(rpn[i]);
        if (op == NULL)
        {
            sprintf(error,  "Unknown operator: %c", rpn[i]);
            return NAN;
        }

        if (op->is_unary)
        {
            if (stack_size < 1)
            {
                sprintf(error, "Not enough operands!");
                return NAN;
            }
            double x = stack[--stack_size];
            stack[stack_size++] = op->func(x, 0);
        }
        else
        {
            if (stack_size < 2)
            {
                sprintf(error, "Not enough operands!");
                return NAN;
            }
            double y = stack[--stack_size];
            double x = stack[--stack_size];
            double res = op->func(x, y);

            if (op->op == '/' && y == 0.0)
            {
                sprintf(error, "Division by zero!");
                return NAN;
            }
            stack[stack_size++] = res;
        }
    }

    if (stack_size != 1)
    {
        sprintf(error, "Invalid expression!");
        return NAN;
    }
    
    update_load_memory();

    return stack[0];
}
