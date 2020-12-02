#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <simple_stack.h>

// Macro para alocação de memória
#define CHUNK 32

// Helper function that allocates and deallocates the heap as needed.
void *maybe_realloc(void *ptr, const size_t used, size_t *alloc) {
    if (*alloc == 0)
        return realloc(ptr, *alloc = CHUNK);

    if (used >= *alloc)
        return realloc(ptr, *alloc *= 2);

    if (used < *alloc / 2 && *alloc > CHUNK)
        return realloc(ptr, *alloc /= 2);
    
    return ptr;
}


struct _stack {
    elem_t *vec;
    size_t alloc;
    uint size;
};

stack_t *stack_create() {
    stack_t *stack = (stack_t *)malloc(sizeof(stack_t));
    stack->vec = NULL;
    stack->alloc = 0;
    stack->size = 0;
    return stack;
}

elem_t stack_pop(stack_t *stack) {
    assert(stack->size > 0);
    elem_t pop = stack->vec[--stack->size];
    stack->vec = (elem_t *)maybe_realloc(stack->vec, stack->size * sizeof(elem_t), &stack->alloc);
    return pop;
}

elem_t stack_peek(stack_t *stack) {
    return stack->vec[stack->size - 1];
}

void stack_push(stack_t *stack, elem_t elem) {
    stack->vec = (elem_t *)maybe_realloc(stack->vec, stack->size * sizeof(elem_t), &stack->alloc);
    stack->vec[stack->size++] = elem;
}

bool stack_is_empty(stack_t *stack) {
    return stack->size == 0;
}

void stack_destroy(stack_t *stack, free_fn_t free_func) {
    while (!stack_is_empty(stack)) {
        elem_t elem = stack_pop(stack);
        free_func(elem);
    }
    free(stack->vec);
    free(stack);
}
