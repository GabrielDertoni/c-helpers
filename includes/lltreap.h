/**
 * Left Leaning Treap Module
 *
 * This module implements a Treap Abstract Data Type that will always favor
 * left rotation in element removal.
 */


#ifndef __LLTREAP_H__
#define __LLTREAP_H__

#include <stdlib.h>
#include <stdbool.h>

// A node in the lltreap structure.
typedef struct _lltreap_node lltreap_node_t;

// The lltreap Abstract Data Type handle.
typedef struct _lltreap lltreap_t;

// A function that is able to compare two elements of the treap.
typedef int (*comp_fn_t)(void *, void *);

// A function that is able to free a single element of the treap.
typedef void (*free_fn_t)(void *);

// A function that takes as input an element and some aditional optional argument.
typedef void (*elem_fn_t)(void *, void *);

// A function that takes as input a node and some aditional optional argument.
typedef void (*node_fn_t)(lltreap_node_t *, void *);

/**
 * Creates the Treap Abstract Data Type
 *
 * @param elsize - the size of the elements to be stored in the treap.
 * @param comp - the comparison function to use to construct the structure.
 * @return the newly created treap. [ownership]
 */
lltreap_t *lltreap_create(size_t elsize, comp_fn_t comp);

/**
 * Frees the Treap data structure and all element in it.
 *
 * @param treap - the Treap to delete. [ownership]
 * @param free_fn - a function to free a pointer to the element.
 */
void lltreap_delete(lltreap_t *treap, free_fn_t free_fn);

/**
 * Searches for an element in the Treap.
 *
 * @param treap - the Treap to search inside. [ref]
 * @param element - the element to search for (with `comp` function). [ref]
 * @return NULL if no element is found and the element if it is. [ref]
 */
void *lltreap_search(lltreap_t *treap, void *element);

/**
 * Inserts an element into the Treap.
 *
 * @param treap - the Treap to insert the element in. [mut ref]
 * @param element - the element to insert. [ref]
 * @return true if the insertion is successfull, false if it is a duplicate.
 */
bool lltreap_insert(lltreap_t *treap, void *element);

/**
 * Inserts an element with a certain priority in the Treap.
 *
 * @param treap - the Treap to insert the element in. [mut ref]
 * @param element - the element to insert. [ref]
 * @param priority - the priority of the inserted element.
 * @return true if the insertion is successfull, false if it is a duplicate.
 */
bool lltreap_insert_with_priority(lltreap_t *treap, void *element, int priority);

/**
 * Removes an element from the Treap.
 *
 * @param treap - the Treap to remove the element from. [mut ref]
 * @param element - the element to search and remove. [ref]
 * @param free_fn - the function to free the found element pointer.
 * @return true if the removal is successfull, false if element is not found.
 */
bool lltreap_remove(lltreap_t *treap, void *element, free_fn_t free_fn);

/**
 * Gets the number of elements in the Treap.
 *
 * @param treap - the Treap to get the size from. [ref]
 * @return the size of the Treap.
 */
size_t lltreap_get_size(lltreap_t *treap);

/**
 * Functions of type: lltreap_<order>_foreach
 *
 * Iterate through every element in the Treap according to <order> and then call
 * `func` with an element as its first argument and `args` as the second.
 *
 * @param treap - the Treap to traverse. [ref]
 * @param func - the function to call in each element.
 * @param args - optional arguments to be sent to `func`. [mut ref]
 */
void lltreap_inorder_foreach(lltreap_t *treap, elem_fn_t func, void *args);
void lltreap_preorder_foreach(lltreap_t *treap, elem_fn_t func, void *args);
void lltreap_postorder_foreach(lltreap_t *treap, elem_fn_t func, void *args);
void lltreap_bfs_foreach(lltreap_t *treap, elem_fn_t func, void *args);

/**
 * Functions of type: lltreap_<order>_foreach_node
 *
 * Iterate through every node in the Treap according to <order> and then call
 * `func` with a node as its first argument and `args` as the second.
 *
 * @param treap - the Treap to traverse. [ref]
 * @param func - the function to call in each node.
 * @param args - optional arguments to be sent to `func`. [mut ref]
 */
void lltreap_inorder_foreach_node(lltreap_t *treap, node_fn_t func, void *args);
void lltreap_preorder_foreach_node(lltreap_t *treap, node_fn_t func, void *args);
void lltreap_postorder_foreach_node(lltreap_t *treap, node_fn_t func, void *args);
void lltreap_bfs_foreach_node(lltreap_t *treap, node_fn_t func, void *args);

/* Node operations */

/**
 * Get the priority of a Treap node.
 *
 * @param node - the node to get the priority from. [ref]
 * @return the priority of the node.
 */
int lltreap_node_priority(lltreap_node_t *node);

/**
 * Get the value of a Treap node.
 *
 * @param node - the node to get the value from. [ref]
 * @return the value of the node.
 */
void *lltreap_node_value(lltreap_node_t *node);

#endif
