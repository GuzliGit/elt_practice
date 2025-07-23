#include "p_queue.h"
#include <stdio.h>
#include <stdlib.h>

void init_queue(PriorityQueue* queue)
{
    for (int i = 0; i < PRIORITIES_COUNT; i++)
    {
        queue->heads[i] = NULL;
        queue->tails[i] = NULL;
    }
}

int push(PriorityQueue* queue, int data, int priority)
{
    if (priority < 0 || priority >= PRIORITIES_COUNT)
        return 0;

    Q_Element* new_element = malloc(sizeof(Q_Element));
    new_element->data = data;
    new_element->next = NULL;

    if (queue->heads[priority] == NULL)
    {
        queue->heads[priority] = new_element;
        queue->tails[priority] = new_element;
    }
    else
    {
        queue->tails[priority]->next = new_element;
        queue->tails[priority] = new_element;
    }

    return 1;
}

int pop_front(PriorityQueue* queue)
{
    for (int i = 0; i < PRIORITIES_COUNT; i++)
    {
        if (queue->heads[i] != NULL)
        {
            Q_Element* temp = queue->heads[i];
            int data = queue->heads[i]->data;
            queue->heads[i] = temp->next;

            if (queue->heads[i] == NULL) 
            {
                queue->tails[i] = NULL;
            }
            
            free(temp);
            return data;
        }
    }
    return -1;
}

int pop_by_priority(PriorityQueue* queue, int priority)
{
    if (priority < 0 || priority >= PRIORITIES_COUNT)
        return -1;

    if (queue->heads[priority] != NULL)
    {
        Q_Element* temp = queue->heads[priority];
        int data = queue->heads[priority]->data;
        queue->heads[priority] = temp->next;

        if (queue->heads[priority] == NULL) 
        {
            queue->tails[priority] = NULL;
        }
        
        free(temp);
        return data;
    }

    return -1;
}

int pop_by_priority_nl(PriorityQueue* queue, int priority)
{
    if (priority < 0 || priority >= PRIORITIES_COUNT)
        return -1;

    for (int i = priority; i < PRIORITIES_COUNT; i++)
    {
        if (queue->heads[i] != NULL)
        {
            Q_Element* temp = queue->heads[i];
            int data = queue->heads[i]->data;
            queue->heads[i] = temp->next;

            if (queue->heads[i] == NULL) 
            {
                queue->tails[i] = NULL;
            }

            free(temp);
            return data;
        }
    }

    return -1;
}

int peek_by_priority_nl(PriorityQueue* queue, int priority)
{
    if (priority < 0 || priority >= PRIORITIES_COUNT)
        return -1;

    for (int i = priority; i < PRIORITIES_COUNT; i++)
    {
        if (queue->heads[i] != NULL)
        {
            return i;
        }
    }

    return -1;
}

void print_queue(PriorityQueue* queue)
{
    for (int i = 0; i < PRIORITIES_COUNT; i++)
    {
        if (queue->heads[i] != NULL)
        {
            Q_Element* current = queue->heads[i];
            while (current != NULL) 
            {
                printf("Данные: %d Приоритет: %d\n", current->data, i);
                current = current->next;
            }
        }
    }
}
