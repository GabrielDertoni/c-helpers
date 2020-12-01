#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>

typedef struct _queue Queue;

Queue *createQueue();
void pushQueue(Queue *queue, void *element);
void *popQueue(Queue *queue);
void *peekQueue(Queue *queue);
void *topQueue(Queue *queue);
bool isEmptyQueue(Queue *queue);
int getSizeQueue(Queue *queue);
void deleteQueue();

#endif
