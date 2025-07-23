#ifndef P_QUEUE_H
#define P_QUEUE_H

#define PRIORITIES_COUNT 256

typedef struct Q_Element
{
    int data;
    struct Q_Element* next;
} Q_Element;

typedef struct PriorityQueue
{
    Q_Element* heads[PRIORITIES_COUNT];
    Q_Element* tails[PRIORITIES_COUNT];
} PriorityQueue;

void init_queue(PriorityQueue*);

int push(PriorityQueue*, int, int);

int pop_front(PriorityQueue*);

int pop_by_priority(PriorityQueue*, int);

int pop_by_priority_nl(PriorityQueue*, int);

int peek_by_priority_nl(PriorityQueue*, int);

void print_queue(PriorityQueue*);

#endif