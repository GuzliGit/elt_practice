#ifndef PRODUCER_H
#define PRODUCER_H

#include "p_manager.h"

#define BUF_SIZE 256
#define MAX_NUM 1024
#define MAX_DIGITS_PER_LINE 10

static void generate_digits(char*, int);

void produce(prod_file*);

#endif