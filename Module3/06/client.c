#include "msgq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Задайте id клиента\n");
        return EXIT_FAILURE;
    }

    long id = atol(argv[1]);

    key_t key;
    int msqid;
    get_msgq(&key, &msqid, 0);

    struct msgbuf msg;
    msg.mtype = 10;
    strcpy(msg.mtext, "connect");
    msg.receiver = -1;
    msg.sender = id;

    msgsnd(msqid, &msg, sizeof(msg), MSG_NOERROR);

    char receiver_ch[MAX_MSG_LEN];
    pid_t pid;

    switch (pid = fork()) 
    {
        case -1:
            perror("process create error");
            exit(EXIT_FAILURE);

        case 0:
            while(1) 
            {
                if (msgrcv(msqid, &msg, sizeof(msg), id, 0) != -1) 
                {
                    printf("\n[Сообщение от %ld]: %s\n", msg.sender, msg.mtext);
                    fflush(stdout);
                }
            }

        default:
            while(1)
            {
                printf("Введите id получателя (20,30,... или -1 для всех): ");
                if (fgets(receiver_ch, MAX_MSG_LEN, stdin) == NULL)
                {
                    perror("receiver input error");
                    exit(EXIT_FAILURE);
                }
            
                msg.receiver = atol(receiver_ch);
                printf("Текст: ");
                if (fgets(msg.mtext, MAX_MSG_LEN, stdin) == NULL)
                {
                    perror("message input error");
                    exit(EXIT_FAILURE);
                }
            
                msg.mtype = 10;
                msg.sender = id;
                if (strlen(msg.mtext) > 0)
                {
                    msg.mtext[strcspn(msg.mtext, "\n")] = '\0';
                    msgsnd(msqid, &msg, sizeof(msg), MSG_NOERROR);
                }
            }
    }

    return EXIT_SUCCESS;
}