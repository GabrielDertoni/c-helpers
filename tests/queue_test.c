#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <colors.h>
#include <tests.h>
#include <queue.h>

int test_counter = 0;

void add_and_remove_empty(void) {
    Queue *queue = createQueue();
    assert(isEmptyQueue(queue));

    void *data = malloc(10 * sizeof(char));
    memcpy(data, "Hello", 6 * sizeof(char));
    pushQueue(queue, data);

    void *data2 = popQueue(queue);
    assert(data == data2);

    assert(!strcmp(data, data2));
    assert(isEmptyQueue(queue));

    deleteQueue(queue);

    free(data2);
}

void filo_structure(void) {
    Queue *queue = createQueue();
    for (int i = 0; i < 100; i++) {
        int *ptr = (int *)malloc(sizeof(int));
        *ptr = i;
        pushQueue(queue, (void *)ptr);
    }

    for (int i = 0; i < 100; i++) {
        int *ptr = (int *)popQueue(queue);
        assert(*ptr == i);
        free(ptr);
    }
    assert(isEmptyQueue(queue));
    deleteQueue(queue);
}

void peeking_doesnt_dequeue(void) {
    Queue *queue = createQueue();
    for (int i = 0; i < 10; i++) {
        int *ptr = (int *)malloc(sizeof(int));
        *ptr = i * i;
        pushQueue(queue, (void *)ptr);
    }
    int size = getSizeQueue(queue);
    int *peek = peekQueue(queue);
    assert(size == getSizeQueue(queue));
    assert(*peek == 0);

    while (!isEmptyQueue(queue))
        free(popQueue(queue));

    deleteQueue(queue);
}

int main(void) {
    TEST_SETUP();

    TEST(add_and_remove_empty());
    TEST(filo_structure());
    TEST(peeking_doesnt_dequeue());

    TEST_TEARDOWN();

    return EXIT_SUCCESS;
}
