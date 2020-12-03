/**
 * Queue Memory Bank
 *
 * This module implements a Queue data structure in a memory bank. This reduces
 * the ammount of mallocs and increases caching efficiency.
 *
 * NOTE: When a value is pushed into the queue, it is copied with memcpy into the
 *       structure. When the element is retreived with queue_pop, a pointer to
 *       the element is returned. This pointer can not be freed directly because
 *       it points to part of the queue structure. However if the element has
 *       some internal fields, those may be freed.
 */

#ifndef __QUEUE_BANK_H__
#define __QUEUE_BANK_H__

#include <stdlib.h>
#include <stdbool.h>

typedef struct _queue queue_t;

/**
 * Creates a new queue.
 *
 * @param elsize - the size of the elements that will be stored in the queue.
 * @return the newly created queue struct. [ownership]
 */
queue_t *queue_create(size_t elsize);

/**
 * Creates a new queue with a starting capacity.
 *
 * @param elsize - the size of the elements that will be stored in the queue.
 * @param cap - the starting capacity (cap * elsize bytes qwill be allocated).
 * @return the newly created queue struct. [ownership]
 */
queue_t *queue_create_with_cap(size_t elsize, size_t cap);

/**
 * Pushes a new element into the top of the queue.
 *
 * @param queue - the queue to push the element into. [mut ref]
 * @param element - the element to push in the queue. [ref]
 */
void queue_push(queue_t *queue, void *element);

/**
 * Pops a value from the bottom of the queue.
 * NOTE: The pointer that is returned can not be freed. However, the structure
 *       that it points to can and must be freed after pop.
 *
 * @param queue - the queue to pop from. [mut ref]
 * @return a pointer to the element popped. [mut ref]
 */
void *queue_pop(queue_t *queue);

/**
 * Returns the value at the bottom of the queue but does not remove it.
 *
 * @param queue - the queue to peak in. [ref]
 * @return a pointer to the element popped. [mut ref]
 */
void *queue_peak(queue_t *queue);

/**
 * Frees the queue structure.
 * NOTE: Does not free elements in the queue, make sure to have an empty queue
 *       before freeing it.
 *
 * @param queue - the queue to free. [ownership]
 */
void queue_delete(queue_t *queue);

/**
 * Gets the number of elements in the queue.
 *
 * @param queue - the queue to get the size from. [ref]
 * @return the size of the queue.
 */
size_t queue_get_size(queue_t *queue);

/**
 * Verifies if the queue is empty.
 *
 * @param queue - the queue to check. [ref]
 * @return true if queue size is 0, false otherwise.
 */
bool queue_is_empty(queue_t *queue);

#endif
