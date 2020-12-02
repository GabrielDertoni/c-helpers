/**
 * Author: Gabriel Dertoni
 * Github: https://github.com/GabrielDertoni
 *
 * Simple sequential stack implementation.
 * This implementation uses heap allocation and reallocs in order to increase
 * and decrease the ammount that is used. It is build around the typedef elem_t
 * which allow for type changes without need for rewriting any part of the
 * implementation.
 */


#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>

typedef void *elem_t;
typedef void (*free_fn_t)(elem_t);

typedef struct _stack stack_t;

/**
 * Creates a new stack
 *
 * @return the newly created (allocated) stack pointer.
 */
stack_t *stack_create();

/**
 * Pops an item out of the stack in a First In First Out (FIFO) manner.
 *
 * @param stack - the stack to pop from.
 * @return the element poped from the stack.
 */
elem_t stack_pop(stack_t *stack);

/**
 * Returns the item at the top of the stack, but does not remove it from the
 * stack.
 *
 * @param stack - the stack to peek at.
 * @return the element at the top of the stack.
 */
elem_t stack_peek(stack_t *stack);

/**
 * Pushes an element on the top of the stack.
 *
 * @param stack - the stack to push the element into.
 * @param elem - the element to be pushed on to the stack.
 */
void stack_push(stack_t *stack, elem_t elem);

/**
 * Checks if stack is empty or not.
 *
 * @param stack - the stack that can be empty.
 * @return true if the stack is empty, false otherwise.
 */
bool stack_is_empty(stack_t *stack);

/**
 * Frees the stack and any remaining elements on it.
 *
 * @param stack - the stack that will be freed.
 * @param free_func - a function that is able to free elem_t
 */
void stack_destroy(stack_t *stack, free_fn_t free_func);

#endif
