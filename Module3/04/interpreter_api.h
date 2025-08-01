#ifndef INTERPRETER_API_H
#define INTERPRETER_API_H

#include "com_interpreter.h"
void prepare_interpreter();

void print_invite();

static void init_instructions(Instruction*);

void run(char*);

#endif