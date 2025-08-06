#ifndef CONSUMER_H
#define CONSUMER_H

#include "p_manager.h"

#define BUF_SIZE 256

static int get_max_int(char*, ssize_t*);

void consume(prod_file*);

#endif