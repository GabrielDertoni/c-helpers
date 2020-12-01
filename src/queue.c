#include <stdlib.h>
#include <stdbool.h>
#include <queue.h>

#include <stdio.h>

#define CHUNK 32

typedef struct {
    u_int32_t next;
    void *value;
} Node;

struct _queue {
    Node *membank;
    u_int32_t start;
    u_int32_t end;
    u_int32_t empty;
    u_int32_t size;
    size_t alloc;
};

Queue *createQueue() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->alloc = 0;
    queue->membank = NULL;
    queue->start = 0;
    queue->end = 0;
    queue->size = 0;
    queue->empty = 0;
    return queue;
}

void pushQueue(Queue *queue, void *element) {
    if (queue->size >= queue->alloc / sizeof(Node)) {
        if ((queue->alloc *= 2) == 0)
            queue->alloc = CHUNK * sizeof(Node);

        queue->membank = (Node *)realloc(queue->membank, queue->alloc);
        queue->empty = queue->size;
        for (u_int32_t i = queue->empty; i < queue->alloc / sizeof(Node); i++)
            if (i == queue->alloc / sizeof(Node) - 1)
                queue->membank[i].next = -1U;
            else
                queue->membank[i].next = i + 1;
    }
    if (queue->size > 0)
        queue->membank[queue->end].next = queue->empty;

    // Inserts element at the end of the queue.
    queue->membank[queue->empty].value = element;
    // End is the old empty space.
    queue->end = queue->empty;
    // Moves empty pointer.
    queue->empty = queue->membank[queue->end].next;
    // Sets next of end to "null" value.
    queue->membank[queue->end].next = -1U;
    queue->size++;
}

void *popQueue(Queue *queue) {
    /* Implement logic to free memory as needed. */
    void *retr = queue->membank[queue->start].value;
    u_int32_t next_start = queue->membank[queue->start].next;
    queue->membank[queue->start].next = queue->empty;
    queue->empty = queue->start;
    queue->start = next_start;
    queue->size--;
    return retr;
}

void *peekQueue(Queue *queue) {
    return queue->membank[queue->start].value;
}

void *topQueue(Queue *queue) {
    return queue->membank[queue->end].value;
}

bool isEmptyQueue(Queue *queue) {
    return queue->size == 0;
}

int getSizeQueue(Queue *queue) {
    return (int)queue->size;
}

void deleteQueue(Queue *queue) {
    free(queue->membank);
    free(queue);
}
