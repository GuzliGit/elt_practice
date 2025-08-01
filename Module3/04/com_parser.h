#ifndef COM_PARSER_H
#define COM_PARSER_H

#include "com_interpreter.h"

void capture_redirect(char*, int*, int*, int*);

int parse(char*, Instruction[], int*);

#endif