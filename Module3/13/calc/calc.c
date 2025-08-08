#include "calc.h"
#include "lexer.h"
#include "parser.h"
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

const Operation operations[] = 
{
    {'+', sum, 0},
    {'-', substraction, 0},
    {'*', multiply, 0},
    {'/', divide, 0},
    {'~', un_minus, 1}
};

double sum(double x, double y)
{
    return x + y;
}

double substraction(double x, double y)
{
    return x - y;
}

double multiply(double x, double y)
{
    return x * y;
}

double divide(double x, double y)
{
    return x / y;
}

double un_minus(double x, double _)
{
    return -x;
}

const Operation* get_op(char op)
{
    for (int i = 0; i < sizeof(operations)/sizeof(operations[0]); i++)
    {
        if (operations[i].op == op)
        {
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

    return stack[0];
}
