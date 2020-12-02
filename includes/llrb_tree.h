#ifndef __LLRB_TREE_H__
#define __LLRB_TREE_H__

#include <stdlib.h>
#include <stdbool.h>

typedef struct _llrb_tree llrb_tree_t;
typedef int (*comp_fn_t)(void *, void *);
typedef void (*free_fn_t)(void *);
typedef void (*foreach_fn_t)(void *, void *);
typedef void *(*map_fn_t)(void *, void *);

llrb_tree_t *llrb_create(size_t elsize, comp_fn_t comp);

void *llrb_successor(llrb_tree_t *tree, void *element);
void *llrb_predecessor(llrb_tree_t *tree, void *element);
void *llrb_max(llrb_tree_t *tree);
void *llrb_min(llrb_tree_t *tree);

void llrb_preorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args);
void llrb_postorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args);
void llrb_inorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args);

void *llrb_search(llrb_tree_t *tree, void *element);
bool llrb_insert(llrb_tree_t *tree, void *element);
// bool llrb_remove(llrb_tree_t *tree, void *element);

void llrb_delete(llrb_tree_t *tree, free_fn_t free_fn);

#endif
