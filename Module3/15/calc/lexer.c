#include "lexer.h"
#include <ctype.h>
#include <string.h>

TokenBuffer buffer[MAX_TOKENS];
int tokens_counter;

static int is_unary_minus()
{
    if (tokens_counter == 0 || buffer[tokens_counter - 1].type == PLUS ||
        buffer[tokens_counter - 1].type == MINUS || buffer[tokens_counter - 1].type == MULT ||
        buffer[tokens_counter - 1].type == DIV || buffer[tokens_counter - 1].type == LBRACKET)
    {
        return 1;
    }
    
    return 0;
}

int tokenize(char* input)
{
    tokens_counter = 0;
    buffer[tokens_counter].val[0] = '\0';

    for (int i = 0; i < strlen(input); i++)
    {
        if (input[i] == ' ')
            continue;

        if (isdigit(input[i]) || input[i] == '.')
        {
            int dig_start = i;
            while (i < strlen(input) && (isdigit(input[i]) || input[i] == '.'))
            {
                i++;
            }

            if ((i - dig_start) > MAX_TOKEN_LEN)
                return 0;

            buffer[tokens_counter].type = NUM;
            strncpy(buffer[tokens_counter].val, &input[dig_start], i - dig_start);
            buffer[tokens_counter].val[i - dig_start] = '\0';
            tokens_counter++;
            i--;
            continue;
        }
        
        switch (input[i]) 
        {
            case '+':
                buffer[tokens_counter].type = PLUS;
                break;
            case '-':
                if (is_unary_minus())
                {
                    buffer[tokens_counter].type = UN_MINUS;
                }
                else
                {
                    buffer[tokens_counter].type = MINUS;
                }
                break;
            case '*':
                buffer[tokens_counter].type = MULT;
                break;
            case '/':
                buffer[tokens_counter].type = DIV;
                break;
            case '(':
                buffer[tokens_counter].type = LBRACKET;
                break;
            case ')':
                buffer[tokens_counter].type = RBRACKET;
                break;
            default:
                buffer[tokens_counter].type = ERR;
                break;;
        }

        buffer[tokens_counter].val[0] = input[i];
        buffer[tokens_counter].val[1] = '\0';
        tokens_counter++;

        if (tokens_counter >= MAX_TOKENS)
            break;
    }

    return 1;
}

TokenBuffer* get_tokens_buf()
{
    return buffer;
}

int get_tokens_count()
{
    return tokens_counter;
}
