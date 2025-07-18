#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

#define MAX_RPN_LEN 2000

typedef enum Priority
{
    P_HIGHEST,
    P_LOW,
    P_MEDIUM,
    P_HIGH,
    P_PREFIX
} Priority;

int get_priority(Token);

char get_char_from_token(Token);

int is_tokens_correct(TokenBuffer*, int, char*);

int get_rpn(TokenBuffer*, int, char*, char*);

#endif
