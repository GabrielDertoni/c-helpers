/**
 * Left Leaning avl Module
 *
 * This module implements a avl Abstract Data Type that will always favor
 * left rotation in element removal.
 */


#ifndef __avl_H__
#define __avl_H__

#include <stdlib.h>
#include <stdbool.h>

// A node in the avl structure.
typedef struct _avl_node avl_node_t;

// The avl Abstract Data Type handle.
typedef struct _avl avl_t;

// A function that is able to compare two elements of the avl.
typedef int (*comp_fn_t)(void *, void *);

// A function that is able to free a single element of the avl.
typedef void (*free_fn_t)(void *);

// A function that takes as input an element and some aditional optional argument.
typedef void (*elem_fn_t)(void *, void *);

// A function that takes as input a node and some aditional optional argument.
typedef void (*node_fn_t)(avl_node_t *, void *);

/**
 * Creates the avl Abstract Data Type
 *
 * @param elsize - the size of the elements to be stored in the avl.
 * @param comp - the comparison function to use to construct the structure.
 * @return the newly created avl. [ownership]
 */
avl_t *avl_create(size_t elsize, comp_fn_t comp);

/**
 * Frees the avl data structure and all element in it.
 *
 * @param avl - the Treap to delete. [ownership]
 * @param free_fn - a function to free a pointer to the element.
 */
void avl_delete(avl_t *avl, free_fn_t free_fn);

/**
 * Searches for an element in the avl.
 *
 * @param avl - the Treap to search inside. [ref]
 * @param element - the element to search for (with `comp` function). [ref]
 * @return NULL if no element is found and the element if it is. [ref]
 */
void *avl_search(avl_t *avl, void *element);

/**
 * Inserts an element into the avl.
 *
 * @param avl - the Treap to insert the element in. [mut ref]
 * @param element - the element to insert. [ref]
 * @return true if the insertion is successfull, false if it is a duplicate.
 */
bool avl_insert(avl_t *avl, void *element);

/**
 * Removes an element from the avl.
 *
 * @param avl - the Treap to remove the element from. [mut ref]
 * @param element - the element to search and remove. [ref]
 * @param free_fn - the function to free the found element pointer.
 * @return true if the removal is successfull, false if element is not found.
 */
bool avl_remove(avl_t *avl, void *element, free_fn_t free_fn);

/**
 * Gets the number of elements in the avl.
 *
 * @param avl - the Treap to get the size from. [ref]
 * @return the size of the avl.
 */
size_t avl_get_size(avl_t *avl);

/**
 * Functions of type: avl_<order>_foreach
 *
 * Iterate through every element in the avl according to <order> and then call
 * `func` with an element as its first argument and `args` as the second.
 *
 * @param avl - the Treap to traverse. [ref]
 * @param func - the function to call in each element.
 * @param args - optional arguments to be sent to `func`. [mut ref]
 */
void avl_inorder_foreach(avl_t *avl, elem_fn_t func, void *args);
void avl_preorder_foreach(avl_t *avl, elem_fn_t func, void *args);
void avl_postorder_foreach(avl_t *avl, elem_fn_t func, void *args);
void avl_bfs_foreach(avl_t *avl, elem_fn_t func, void *args);

/**
 * Functions of type: avl_<order>_foreach_node
 *
 * Iterate through every node in the avl according to <order> and then call
 * `func` with a node as its first argument and `args` as the second.
 *
 * @param avl - the Treap to traverse. [ref]
 * @param func - the function to call in each node.
 * @param args - optional arguments to be sent to `func`. [mut ref]
 */
void avl_inorder_foreach_node(avl_t *avl, node_fn_t func, void *args);
void avl_preorder_foreach_node(avl_t *avl, node_fn_t func, void *args);
void avl_postorder_foreach_node(avl_t *avl, node_fn_t func, void *args);
void avl_bfs_foreach_node(avl_t *avl, node_fn_t func, void *args);

/* Node operations */

int avl_node_height(avl_node_t *node);

/**
 * Get the value of a avl node.
 *
 * @param node - the node to get the value from. [ref]
 * @return the value of the node.
 */
void *avl_node_value(avl_node_t *node);

void avl_pretty_print(avl_t *tree);

#endif
