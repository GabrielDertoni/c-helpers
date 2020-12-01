#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <tests.h>
#include <simple_stack.h>

#define WITH_STACK(func) ({ \
    stack_t *__stack = stack_create(); \
    func(__stack); \
    stack_destroy(__stack, free); \
})

void empty_stack(stack_t *s) {
    assert(stack_is_empty(s));
}

void insert_and_remove(stack_t *s) {
    stack_push(s, (void *)strdup("Hello"));
    assert(!stack_is_empty(s));

    char *str = stack_pop(s);
    assert(!strcmp(str, "Hello"));

    assert(stack_is_empty(s));

    free(str);
}

void fifo(stack_t *s) {
    for (int i = 0; i < 100; i++) {
        int *ptr = (int *)malloc(sizeof(int));
        *ptr = i;
        stack_push(s, (void *)ptr);
    }

    for (int i = 99; i >= 0; i--) {
        int *ptr = stack_pop(s);
        assert(*ptr == i);
        free(ptr);
    }
}


int main() {
    TEST_SETUP();

    NAMED_TEST("empty_stack", WITH_STACK(empty_stack));
    NAMED_TEST("insert_and_remove", WITH_STACK(insert_and_remove));
    NAMED_TEST("fifo", WITH_STACK(fifo));

    TEST_TEARDOWN();

    return EXIT_SUCCESS;
}
