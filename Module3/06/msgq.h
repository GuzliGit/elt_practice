#ifndef MSGQ_H
#define MSGQ_H

#include <sys/types.h>

#define GEN_FILE "msg_q.gen"
#define MAX_CLIENTS 100
#define MAX_MSG_LEN 256

typedef struct msgbuf
{
    long mtype;
    long sender;
    long receiver;
    char mtext[MAX_MSG_LEN];
} msgbuf;

void get_msgq(key_t*, int*, int);

#endif