#ifndef COUNTER_H
#define COUNTER_H

#define MSG_LEN_MAX 128

static int sig_count = 0;
static int fd = -1;
static char msg[MSG_LEN_MAX];

static char* get_current_dir();

void write_to_file(int, char*);

void listener(int);

void start_count(char*);

#endif