#include "p_queue.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define GENERATION_RANGE 2000

void generate_queue(PriorityQueue* queue, int count)
{
    printf("\nГенерация данных:\n");
    int data, priority;
    for (int i = 0; i < count; i++)
    {
        data = rand() % GENERATION_RANGE;
        priority = rand() % PRIORITIES_COUNT;

        push(queue, data, priority);
    }
}

void test_priority_order(PriorityQueue* queue)
{
    printf("\nТест 1: Извлечение по приоритету (высший из сгенерированных)\n");
    for (int p = 0; p < PRIORITIES_COUNT; p++) 
    {
        if (queue->heads[p] != NULL) 
        {
            int data = 0;
            while (data != - 1) 
            {
                data = pop_by_priority(queue, p);
                if (data != -1)
                    printf("Извлечено: %d (приоритет %d)\n", data, p);
            }
            return;
        }
    }
}

void test_priority_order_nl(PriorityQueue* queue, int priority)
{
    printf("\nТест 2: Извлечение с приоритетом >= %d\n", priority);
    while (1) 
    {
        int real_priority = peek_by_priority_nl(queue, priority);
        int data = pop_by_priority_nl(queue, priority);
        if (data == -1) 
            break;

        printf("Извлечено: %d (приоритет %d)\n", data, real_priority);
    }
}

int main(int argc, char* argv[])
{
    if (argc == 0 || argc != 2)
    {
        printf("Задайте кол-во сообщений для генерации в формате: [исполняемый файл] [кол-во сообщений].\n");
        return 0;
    }
    
    srand(time(NULL));
    char* endptr;
    long count = strtol(argv[argc - 1], &endptr, 10);

    if (argv[argc - 1] == endptr || *endptr != '\0')
    {
        printf("Введите число!\n");
        return 0;
    }
    else if (count <= 0)
    {
        printf("Заданное число сообщений должно быть больше 0!\n");
        return 0;
    }

    PriorityQueue queue;
    init_queue(&queue);

    generate_queue(&queue, count);
    print_queue(&queue);

    test_priority_order(&queue);

    test_priority_order_nl(&queue, 200);

    return 1;
}
