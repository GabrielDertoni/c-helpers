#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <queue_bank.h>

typedef struct {
    uint next;
} node_t;

struct _queue {
    node_t *nodes;
    void *data;
    uint start;
    uint end;
    uint empty;
    uint size;
    size_t cap;
    size_t elsize;
};

queue_t *queue_create(size_t elsize) {
    queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
    queue->cap = 0;
    queue->elsize = elsize;
    queue->nodes = NULL;
    queue->data = NULL;
    queue->start = 0;
    queue->end = 0;
    queue->size = 0;
    queue->empty = 0;
    return queue;
}

queue_t *queue_create_with_cap(size_t elsize, size_t cap) {
    queue_t *queue = queue_create(elsize);
    queue->cap = cap;
    queue->nodes = (node_t *)malloc(cap * sizeof(node_t));
    queue->data = malloc(elsize * cap);

    for (int i = 0; i < queue->cap; i++)
        if (i == queue->cap - 1)
            queue->nodes[i].next = -1U;
        else
            queue->nodes[i].next = i + 1;

    return queue;
}

inline static void *_index(queue_t *queue, size_t i) {
    return queue->data + i * queue->elsize;
}

void queue_push(queue_t *queue, void *element) {
    if (queue->size >= queue->cap) {
        // From empty queue, allocate space for 2 elements.
        if ((queue->cap *= 2) == 0) queue->cap = 2;

        queue->nodes = (node_t *)realloc(queue->nodes, queue->cap * sizeof(node_t));
        queue->data = realloc(queue->data, queue->cap * queue->elsize);

        queue->empty = queue->size;
        for (int i = queue->empty; i < queue->cap; i++)
            if (i == queue->cap - 1)
                queue->nodes[i].next = -1U;
            else
                queue->nodes[i].next = i + 1;
    }
    if (queue->size > 0)
        queue->nodes[queue->end].next = queue->empty;

    // Copies element at the end of the queue.
    memcpy(_index(queue, queue->empty), element, queue->elsize);
    // End is the old empty space.
    queue->end = queue->empty;
    // Moves empty pointer.
    queue->empty = queue->nodes[queue->empty].next;
    // Sets next of end to "null" value.
    queue->nodes[queue->end].next = -1U;
    queue->size++;
}

void *queue_pop(queue_t *queue) {
    // TODO: Implement logic to free memory as needed.
    void *retr = _index(queue, queue->start);

    uint next_start = queue->nodes[queue->start].next;
    queue->nodes[queue->start].next = queue->empty;
    queue->empty = queue->start;

    if (next_start != -1U)
        queue->start = next_start;

    queue->size--;
    return retr;
}

void *queue_peak(queue_t *queue) {
    return _index(queue, queue->start);
}

// NOTE: Does not free remaining data.
void queue_delete(queue_t *queue) {
    free(queue->nodes);
    free(queue->data);
    free(queue);
}

size_t queue_get_size(queue_t *queue) {
    return queue->size;
}

bool queue_is_empty(queue_t *queue) {
    return queue->size == 0;
}

