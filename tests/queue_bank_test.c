#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "test_utils.h"
#include <queue_bank.h>

bool test_queue_bank_peak() {
    queue_t *queue = queue_create(sizeof(int));
    assert_neq(queue, NULL);

    for (int i = 0; i < 100; i++)
        queue_push(queue, &i);

    assert_eq(queue_get_size(queue), 100);

    for (int i = 0; i < 10; i++)
        assert_eq(*(int *)queue_peak(queue), 0);

    assert_eq(queue_get_size(queue), 100);

    queue_delete(queue);
    return true;
}

bool test_queue_bank_pop() {
    queue_t *queue = queue_create(sizeof(int));
    assert_neq(queue, NULL);

    for (int i = 0; i < 100; i++)
        queue_push(queue, &i);

    assert_eq(queue_get_size(queue), 100);

    for (int i = 0; i < 100; i++)
        assert_eq(*(int *)queue_pop(queue), i);

    assert_eq(queue_get_size(queue), 0);

    queue_delete(queue);
    return true;
}

bool test_queue_bank_push() {
    int val;
    queue_t *queue = queue_create(sizeof(int));
    assert_neq(queue, NULL);

    assert_eq(queue_get_size(queue), 0);
    assert_eq(queue_is_empty(queue), true);

    val = 10;
    queue_push(queue, &val);
    assert_eq(queue_get_size(queue), 1);
    assert_eq(queue_is_empty(queue), false);

    val = 28;
    queue_push(queue, &val);
    assert_eq(queue_get_size(queue), 2);
    
    val = 17;
    queue_push(queue, &val);
    assert_eq(queue_get_size(queue), 3);

    for (int i = 0; i < 100; i++) {
        queue_push(queue, &i);
        assert_eq(queue_get_size(queue), i + 4);
    }

    queue_delete(queue);
    return true;
}

bool test_queue_bank_create() {
    queue_t *queue = queue_create(sizeof(int));
    assert_neq(queue, NULL);

    assert_eq(queue_get_size(queue), 0);
    assert_eq(queue_is_empty(queue), true);

    queue_delete(queue);
    return true;
}

int main(void) {
    TEST_SETUP();

    test_fn(test_queue_bank_create());
    test_fn(test_queue_bank_push());
    test_fn(test_queue_bank_pop());
    test_fn(test_queue_bank_peak());

    TEST_TEARDOWN();
    return EXIT_SUCCESS;
}
