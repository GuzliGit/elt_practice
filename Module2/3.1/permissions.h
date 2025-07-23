#ifndef PERMISSIONS_H
#define PERMISSIONS_H

#include <sys/types.h>

void print_sym(mode_t);

void print_dig(mode_t);

void print_bin(mode_t);

int exec_chmod_c(char*, int, char*[]);

#endif