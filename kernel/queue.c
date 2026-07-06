#include "queue.h"
#include "../hal/uart.h"

void queue_init(Queue *q) {
    q->head  = 0;
    q->tail  = 0;
    q->count = 0;
}

int queue_push(Queue *q, uint32_t value) {
    if (q->count >= QUEUE_SIZE) {
        uart_puts("[QUEUE] Full — drop\n");
        return 0;
    }
    q->data[q->tail] = value;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count++;
    return 1;
}

int queue_pop(Queue *q, uint32_t *value) {
    if (q->count == 0) return 0;
    *value = q->data[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;
    return 1;
}

int queue_is_empty(Queue *q) {
    return q->count == 0;
}