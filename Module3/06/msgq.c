#include "msgq.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void get_msgq(key_t* key, int* msqid, int is_create)
{
    *key = ftok(GEN_FILE, 'A');
    if (*key == -1)
    {
        perror("key generation error");
        exit(EXIT_FAILURE);
    }

    *msqid = msgget(*key, (is_create ? IPC_CREAT : 0) | 0666);
    if (*msqid == -1)
    {
        perror("msg queue create error");
        exit(EXIT_FAILURE);
    }
}