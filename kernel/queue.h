#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>

#define QUEUE_SIZE 16

typedef struct {
    uint32_t data[QUEUE_SIZE];
    int      head;
    int      tail;
    int      count;
} Queue;

void queue_init(Queue *q);
int  queue_push(Queue *q, uint32_t value);
int  queue_pop(Queue *q, uint32_t *value);
int  queue_is_empty(Queue *q);

#endif