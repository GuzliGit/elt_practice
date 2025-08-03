#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <termios.h>
#include <unistd.h>

#define Q1to2 "/q1to2"
#define Q2to1 "/q2to1"
#define MAX_MSGS 10
#define MAX_MSG_SIZE 256
#define EXIT_CODE 255

int usr_id;

int receive_msg(mqd_t mq_rcv, char* input, unsigned int* priority)
{
    if (mq_receive(mq_rcv, input, MAX_MSG_SIZE, priority) == -1)
    {
        perror("receive error");
        return 0;
    }
    if (*priority == EXIT_CODE)
    {
        printf("Клиент %d завершил чат...\n", usr_id == 1 ? 2 : 1);
        return 0;
    }

    printf("%d>> %s\n", usr_id == 1 ? 2 : 1, input);
    return 1;
}

void start_chat()
{
    struct mq_attr q_attrs;
    q_attrs.mq_flags = 0;
    q_attrs.mq_curmsgs = 0;
    q_attrs.mq_maxmsg = MAX_MSGS;
    q_attrs.mq_msgsize = MAX_MSG_SIZE; 
    
    mqd_t mq_snd = mq_open(usr_id == 1 ? Q1to2 : Q2to1, O_CREAT | O_WRONLY, 0666, &q_attrs);
    mqd_t mq_rcv = mq_open(usr_id == 1 ? Q2to1 : Q1to2, O_CREAT | O_RDONLY, 0666, &q_attrs);

    if (mq_snd == (mqd_t)-1 || mq_rcv == (mqd_t)-1)
    {
        perror("queue creating error");
        exit(EXIT_FAILURE);
    }

    char input[MAX_MSG_SIZE];
    unsigned int priority;

    printf("==Клиент %d==\n(введите exit для выхода)\n", usr_id);

    while (1) 
    {
        if (usr_id == 2)
        {
            if (!receive_msg(mq_rcv, input, &priority))
                break;
            tcflush(STDIN_FILENO, TCIFLUSH);
        }

        printf("%d>> ", usr_id);
        fgets(input, MAX_MSG_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) 
        {
            mq_send(mq_snd, "", 0, EXIT_CODE);
            break;
        }

        if (mq_send(mq_snd, input, strlen(input) + 1, 1) == -1)
        {
            perror("send error");
            break;
        }

        if (usr_id == 1)
        {
            if (!receive_msg(mq_rcv, input, &priority))
                break;
            tcflush(STDIN_FILENO, TCIFLUSH);
        }
    }

    mq_close(mq_snd);
    mq_close(mq_rcv);
    mq_unlink(usr_id == 1 ? Q1to2 : Q2to1);
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Для использования программы задайте в аргументах приоритет пользователя (1) или (2)\n");
        return EXIT_FAILURE;
    }

    usr_id = atoi(argv[1]);
    if (usr_id != 1 && usr_id != 2)
    {
        printf("Приоритет пользователя должен быть равным (1) или (2)\n");
        return EXIT_FAILURE;
    }

    start_chat();

    return EXIT_SUCCESS;
}