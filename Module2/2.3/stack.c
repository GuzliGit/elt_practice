#include "stack.h"
#include <stdlib.h>

#define STACK_OVERFLOW -100
#define STACK_UNDERFLOW -101

void init_stack(Stack_c* stack)
{
    stack->size = 0;
}

void push(Stack_c* stack, const T val)
{
    if (stack->size >= STACK_SIZE)
    {
        exit(STACK_OVERFLOW);
    }

    stack->data[stack->size] = val;
    stack->size++;
}

T pop(Stack_c* stack)
{
    if (stack->size == 0)
    {
        exit(STACK_UNDERFLOW);
    }

    stack->size--;
    return stack->data[stack->size];
}

T peek(Stack_c* stack)
{
    if (stack->size <= 0)
    {
        exit(STACK_UNDERFLOW);
    }

    return stack->data[stack->size - 1];
}
