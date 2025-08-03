#include "msgq.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>

int msqid;

long clients[MAX_CLIENTS];
int clients_count = 0;

void connect_c(long id)
{
    if (clients_count < MAX_CLIENTS)
    {
        clients[clients_count++] = id;
        printf("Клиент %ld подключен к серверу\n", id);
    }
}

void disconnect_c(long id)
{
    for (int i = 0; i < clients_count; i++)
    {
        if (id == clients[i])
        {
            clients[i] = clients[clients_count - 1];
            clients_count--;
            printf("Клиент %ld отключен от сервера\n", id);
            break;
        }
    }
}

void server_shutdown(int sig)
{
    printf("\nСервер завершил работу...\n");
    if (msgctl(msqid, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl error");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

int main()
{
    key_t key;
    get_msgq(&key, &msqid, 1);

    signal(SIGINT, server_shutdown);
    signal(SIGTERM, server_shutdown);
    printf("Сервер успешно запущен!\n");

    struct msgbuf msg;
    while(1)
    {
        msgrcv(msqid, &msg, sizeof(msg), 10, MSG_NOERROR);

        if (strcmp(msg.mtext, "connect") == 0)
        {
            connect_c(msg.sender);
            continue;
        }
        else if (strcmp(msg.mtext, "shutdown") == 0)
        {
            disconnect_c(msg.sender);
            continue;
        }

        if (msg.receiver > 10)
        {
            for (int i = 0; i < clients_count; i++) 
            {
                if (clients[i] == msg.receiver) 
                {
                    msg.mtype = msg.receiver;
                    msgsnd(msqid, &msg, sizeof(msg), MSG_NOERROR);
                }
            }
        }
        else if (msg.receiver == 10)
        {
            printf("Получено сообщение от клиента %ld.\n", msg.sender);
            printf("%s\n", msg.mtext);
        }
        else if (msg.receiver == -1)
        {
            for (int i = 0; i < clients_count; i++) 
            {
                if (clients[i] != msg.sender) 
                {
                    msg.mtype = clients[i];
                    msgsnd(msqid, &msg, sizeof(msg), MSG_NOERROR);
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
