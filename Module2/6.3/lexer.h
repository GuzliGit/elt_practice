#ifndef LEXER_H
#define LEXER_H

#define MAX_TOKENS 100
#define MAX_TOKEN_LEN 20

typedef enum Token 
{
    NUM,
    PLUS,
    MINUS,
    UN_MINUS,
    MULT,
    DIV,
    LBRACKET,
    RBRACKET,
    ERR
} Token;

typedef struct TokenBuffer 
{
    char val[MAX_TOKEN_LEN];
    Token type;
} TokenBuffer;

static int is_unary_minus();

int tokenize(char*);

TokenBuffer* get_tokens_buf();

int get_tokens_count();

#endif
