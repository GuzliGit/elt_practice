#ifndef STACK_H
#define STACK_H

#include "lexer.h"

#define STACK_SIZE 100

typedef Token T;

typedef struct Stack_custom
{
    T data[STACK_SIZE];
    int size;
} Stack_c;

void init_stack(Stack_c*);

void push(Stack_c*, const T);

T pop(Stack_c*);

T peek(Stack_c*);

#endif