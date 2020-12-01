/**
 * Author: Gabriel Dertoni
 * Github: https://github.com/GabrielDertoni
 *
 * This implementation defines a linked skip list wich means the layers are also
 * linked together.
 */


#ifndef __SKIP_LIST_H__
#define __SKIP_LIST_H__

#include <stdbool.h>
#include <stdlib.h>

#define ELEM_MIN strdup("")
#define ELEM_MAX "ZZZZZZZZZZ"

// The general element type. This may be changed and recompiled to whichever type.
typedef char * elem_t;

// The type used to reference a node in the skip list.
typedef struct _skip_node skip_node_t;

// The type used to reference a skip list.
typedef struct _skip_list skip_list_t;

// A type for functions that are able to free elem_t.
typedef void (*free_fn_t)(elem_t);

// A type for functions that can compare two elem_t and return an integer > 0 if
// the first one is bigger, < 0 if the second one is bigger and 0 if they are equal.
typedef int (*comp_fn_t)(elem_t, elem_t);

/* ======================== SKIP NODE ======================== */

/**
 * Creates a new skip node.
 *
 * @param elem - the element that will be stored in the skip node. [ownership]
 * @param next - a pointer to the next node in the skip list. [ref]
 * @param below - a pointer to a node below this one in the skip list. [ref]
 * @return the newly created (allocated) node. [ownership]
 */
skip_node_t *skip_node_create(elem_t elem, skip_node_t *next, skip_node_t *below);

/* GETTERS */

/**
 * Gets the next node in the skip list.
 *
 * @param node - the node that will be used as reference to get the next. [ref]
 * @return the next node. [ref]
 */
skip_node_t *skip_node_next(skip_node_t *node);

/**
 * Gets the node below in the skip list.
 *
 * @param node - the node that will be used as reference to get the below. [ref]
 * @return the below node. [ref]
 */
skip_node_t *skip_node_below(skip_node_t *node);

/**
 * Gets the value inside the node.
 *
 * @param node - the node to get the value inside. [ref]
 * @return the value inside the node. [ref].
 */
elem_t skip_node_value(skip_node_t *node);

/* METHODS */

/**
 * Destroys a node and returns value inside.
 *
 * @param node - the node to be freed. [ownership]
 * @return the value inside of the node. [ownership].
 */
elem_t skip_node_destroy(skip_node_t *node);

/**
 * Recursively insert an element in the skip list.
 *
 * @param head - the node to be used as a head (will always insert after and never on top of it). [mut ref]
 * @param elem - the element value to be inserted. [ownership]
 * @param comp - the comparison function between two elements. [ref]
 * @return if a node was inserted in the same layer as head, returns the node, else returns NULL. [ref]
 */
skip_node_t *skip_node_insert(skip_node_t *head, elem_t elem, comp_fn_t comp);

/**
 * Recursively remove the first elment that is equal to elem in the skip list.
 *
 * @param head - the node to be used as a head (will always remove after and never on top of it). [mut ref]
 * @param elem - the element to be searched through the list, the first that is found is removed. [ref]
 * @param comp_fn_t - the comparison function between two elements. [ref]
 * @return a linked list of all removed nodes (using skip_node_t but all of them have below = NULL). [ownership]
 */
skip_node_t *skip_node_remove(skip_node_t *head, elem_t elem, comp_fn_t comp);

/* ============ SKIP LIST ============ */

/**
 * Creates a new skip list.
 *
 * @param layer_max - the maximum number of layers that the skip list can have. [ownership]
 * @param comp_fn_t - the comparison function between two elements. [ref]
 * @return the newly created (allocated) skip list. [ownership]
 */
skip_list_t *skip_list_create(uint layer_max, comp_fn_t comp);

/* GETTERS */

/**
 * Gets the head of the skip list.
 *
 * @param l - the skip list. [ref]
 * @return the head of the skip list. [ref]
 */
skip_node_t *skip_list_head(skip_list_t *l);

/**
 * Checks if the skip list is empty
 *
 * @param l - the skip list. [ref]
 * @return if the skip list is empty, returns true, false otherwise. [ref]
 */
bool skip_list_is_empty(skip_list_t *l);

/**
 * Gets the size of the skip list (number of nodes in the bottom layer).
 *
 * @param l - the skip list. [ref]
 * @return the size of the skip list. [ref]
 */
uint skip_list_size(skip_list_t *l);

/* METHODS */

/**
 * Uses the skip list compare function to compare two elements.
 *
 * @param l - the skip list. [ref]
 * @param a - one of the nodes to compare. [ref]
 * @param b - the other of the nodes to compare. [ref]
 * @return 0 if a == b, integer > 0 if a > b and integer < 0 if a < b. [ownership]
 */
int skip_list_compare(skip_list_t *l, skip_node_t *a, skip_node_t *b);

/**
 * The first occurrence of elem in the skip list.
 *
 * @param l - the skip list. [ref]
 * @param elem - the element to be found.
 * @return NULL if the element is not found, a reference to the topmost node to contain elem. [ref]
 */
skip_node_t *skip_list_find(skip_list_t *l, elem_t elem);

/**
 * Inserts an element in the skip list.
 *
 * @param l - the skip list. [mut ref]
 * @param elem - the element to be inserted. [ownership]
 */
void skip_list_insert(skip_list_t *l, elem_t elem);

/**
 * Removes the first occurrence of elem in the skip list.
 *
 * @param l - the skip list. [mut ref]
 * @param elem - the element to be removed. [ref]
 * @return a linked list of nodes that were removed (as skip nodes with below = NULL). [ownership]
 */
skip_node_t *skip_list_remove(skip_list_t *l, elem_t elem);

/**
 * Removes the first occurrence of elem in the skip list and frees the elements.
 *
 * @param l - the skip list. [mut ref]
 * @param elem - the element to be removed. [ref]
 * @param free_func - a function that can free elem_t. [ref]
 * @return true if the element was removed successfully and false if not.
 */
bool skip_list_remove_free(skip_list_t *l, elem_t elem, free_fn_t free_func);

/**
 * Frees the skip list structure and all of the remaining elements.
 *
 * @param l - the skip list. [mut ref]
 * @param free_func - a function that can free elem_t. [ref]
 */
void skip_list_destroy(skip_list_t *l, free_fn_t free_func);


/* ============ HELPERS ============ */

// void stack_free(elem_t elem);
void heap_free(elem_t elem);
int compare(elem_t a, elem_t b);

#endif
