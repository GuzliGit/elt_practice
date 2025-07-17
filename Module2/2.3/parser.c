#include "lexer.h"
#include "parsec.h"
#include "stack.h"
#include <stdio.h>
#include <string.h>

int get_priority(Token t)
{
    switch (t) 
    {
        case PLUS: case MINUS: 
            return P_LOW;
        case MULT: case DIV:
            return P_MEDIUM;
        case UN_MINUS:
            return P_PREFIX;
        case LBRACKET: case RBRACKET:
            return P_HIGHEST;
        default:
            return -1;
    }
}

char get_char_from_token(Token t)
{
    switch (t) 
    {
        case PLUS: 
            return '+';
        case MINUS: 
            return '-';
        case UN_MINUS:
            return '~';
        case MULT: 
            return '*';
        case DIV:
            return '/';
        case LBRACKET: 
            return '(';
        case RBRACKET:
            return ')';
        default:
            return '_';
    }
}

int is_tokens_correct(TokenBuffer* buf, int buf_size, char* err_token)
{
    int unclosed_brackets = 0;

    for (int i = 0; i < buf_size; i++) 
    {
        if (buf[i].type == ERR) 
        {
            sprintf(err_token, "Unknown token: %s", buf[i].val);
            return 0;
        }

        if (buf[i].type == LBRACKET) 
        {
            unclosed_brackets++;
        } else if (buf[i].type == RBRACKET) 
        {
            unclosed_brackets--;

            if (unclosed_brackets < 0) 
            {
                sprintf(err_token, "Unclosed bracket at %d", i);
                return 0;
            }
        }
    }

    if (unclosed_brackets > 0) 
    {
        sprintf(err_token, "Is %d unclosed brackets", unclosed_brackets);
        return 0;
    }

    return 1;
}

int get_rpn(TokenBuffer* tokens, int tokens_count, char* rpn, char* error)
{
    if (!is_tokens_correct(tokens, tokens_count, error))
    {
        return 0;
    }
    
    Stack_c stack;
    init_stack(&stack);

    int res_len = 0;
    for (int i = 0; i < tokens_count; i++)
    {
        if (tokens[i].type == NUM)
        {
            strcpy(rpn + res_len, tokens[i].val);
            res_len += strlen(tokens[i].val); 
            rpn[res_len++] = ' ';
            continue;
        }

        switch (get_priority(tokens[i].type)) 
        {
            case P_HIGHEST:
                if (tokens[i].type == LBRACKET)
                {
                    push(&stack, tokens[i].type);
                }
                else if (tokens[i].type == RBRACKET)
                {
                    while (stack.size > 0 && peek(&stack) != LBRACKET)
                    {
                        rpn[res_len++] = get_char_from_token(pop(&stack));
                        rpn[res_len++] = ' ';
                    }
                    
                    if (stack.size > 0 && peek(&stack) == LBRACKET) 
                    {
                        pop(&stack);
                    }
                }
                break;
            case P_LOW: case P_MEDIUM: case P_HIGH:
                while (stack.size > 0 && peek(&stack) != LBRACKET 
                && get_priority(peek(&stack)) >= get_priority(tokens[i].type))
                {
                    rpn[res_len++] = get_char_from_token(pop(&stack));
                    rpn[res_len++] = ' ';
                }
                push(&stack, tokens[i].type);
                break;
            case P_PREFIX:
                while (stack.size > 0 && peek(&stack) != LBRACKET 
                && get_priority(peek(&stack)) > get_priority(tokens[i].type))
                {
                    rpn[res_len++] = get_char_from_token(pop(&stack));
                    rpn[res_len++] = ' ';
                }
                push(&stack, tokens[i].type);
                break;
        }
    }

    while (stack.size > 0) 
    {
        rpn[res_len++] = get_char_from_token(pop(&stack));
        rpn[res_len++] = ' ';
    }

    rpn[res_len] = '\0';
    return 1;
}