#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue_bank.h>
#include <avl.h>

// Special flag to use when removing an element.
#define NOT_FOUND ((void *)0x1UL)
#define max(a, b) ({ \
    __typeof__(a) __a = (a); \
    __typeof__(b) __b = (b); \
    __typeof__(a) __c; \
    if (__a > __b) __c = __a; \
    else __c = __b; \
    __c; \
})

// Use as shorthand for avl_node_t.
typedef struct _avl_node node_t;
typedef unsigned char byte_t;

struct _avl_node {
    uint height;
    node_t *left;
    node_t *right;
    byte_t value[]; // Flexible array member that stores data.
};

struct _avl {
    node_t *root;
    size_t size;
    size_t elsize;
    comp_fn_t comp;
};

avl_t *avl_create(size_t elsize, comp_fn_t comp) {
    avl_t *avl = (avl_t *)malloc(sizeof(avl_t));
    avl->comp = comp;
    avl->elsize = elsize;
    avl->root = NULL;
    avl->size = 0;
    return avl;
}

static void _delete(node_t *node, free_fn_t free_fn) {
    if (!node) return;
    if (free_fn) free_fn(node->value);
    _delete(node->left, free_fn);
    _delete(node->right, free_fn);
    free(node);
}

void avl_delete(avl_t *avl, free_fn_t free_fn) {
    _delete(avl->root, free_fn);
    free(avl);
}

static node_t *_search(node_t *node, void *element, comp_fn_t comp) {
    if (!node) return NULL;
    int cmp = comp(element, node->value);
    if (cmp > 0) return _search(node->right, element, comp);
    if (cmp < 0) return _search(node->left , element, comp);
    return node;
}

void *avl_search(avl_t *avl, void *element) {
    node_t *node = _search(avl->root, element, avl->comp);
    return node ? node->value : NULL;
}

static node_t *_create_node(void *element, size_t elsize) {
    node_t *node = (node_t *)malloc(sizeof(node_t) + elsize);
    node->height = 1;
    // node->value = memcpy(malloc(elsize), element, elsize);
    memcpy(node->value, element, elsize);
    node->left = NULL;
    node->right = NULL;
    return node;
}

static node_t *_rotate_left(node_t *node) {
    node_t *new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;
    node->height = max(avl_node_height(node->right), avl_node_height(node->left)) + 1;
    new_root->height = max(avl_node_height(new_root->right), avl_node_height(new_root->left)) + 1;
    return new_root;
}

static node_t *_rotate_right(node_t *node) {
    node_t *new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;
    node->height = max(avl_node_height(node->right), avl_node_height(node->left)) + 1;
    new_root->height = max(avl_node_height(new_root->right), avl_node_height(new_root->left)) + 1;
    return new_root;
}

inline static int _get_balance(node_t *node) {
    if (!node) return 0;
    return avl_node_height(node->left) - avl_node_height(node->right);
}

static node_t *_balance(node_t *node) {
    if (!node) return NULL;
    int bal = _get_balance(node);
    node->height = max(avl_node_height(node->right), avl_node_height(node->left)) + 1;
    if (bal < -1) {
        if (_get_balance(node->right) > 0)
            node->right = _rotate_right(node->right);

        node = _rotate_left(node);
    } else if (bal > 1) {
        if (_get_balance(node->left) < 0)
            node->left = _rotate_left(node->left);

        node = _rotate_right(node);
    }
    return node;
}

static node_t *_insert(node_t *node, void *element, size_t elsize, comp_fn_t comp) {
    if (!node) return _create_node(element, elsize);

    int cmp = comp(element, node->value);
    node_t *next;
    if (cmp > 0) {
        next = _insert(node->right, element, elsize, comp);
        if (!next) return NULL;

        node->right = next;

    } else if (cmp < 0) {
        next = _insert(node->left, element, elsize, comp);
        if (!next) return NULL;

        node->left = next;

    } else {
        // Explicitly disallow duplicate keys.
        return NULL;
    }
    return _balance(node);
}

bool avl_insert(avl_t *avl, void *element) {
    node_t *node = _insert(avl->root, element, avl->elsize, avl->comp);
    if (!node) return false;
    avl->root = node;
    avl->size++;
    return true;
}

// Searches for the maximum node from the curr_node subtree. When it is found, replace it
// with the to_remove node.
static node_t *_replace_node(node_t *to_remove, node_t *curr_node, size_t elsize, free_fn_t free_fn) {
    if (curr_node->right) {
        curr_node->right = _replace_node(to_remove, curr_node->right, elsize, free_fn);
        return _balance(curr_node);
    }

    node_t *tmp = curr_node->left;
    if (free_fn) free_fn(to_remove->value);
    memcpy(to_remove->value, curr_node->value, elsize);
    free(curr_node);
    return tmp;
}

static node_t *_remove(node_t *node, void *element, size_t elsize, comp_fn_t comp, free_fn_t free_fn) {
    if (!node) return NOT_FOUND;
    int cmp = comp(element, node->value);
    node_t *next;
    if (cmp > 0) {
        next = _remove(node->right, element, elsize, comp, free_fn);
        if (next == NOT_FOUND) return NOT_FOUND;
        node->right = next;

    } else if (cmp < 0) {
        next = _remove(node->left, element, elsize, comp, free_fn);
        if (next == NOT_FOUND) return NOT_FOUND;
        node->left = next;

    } else {
        if (node->left == NULL || node->right == NULL) {
            next = node->left ? node->left : node->right;
            if (free_fn) free_fn(node->value);
            free(node);
            node = next;
        } else {
            node = _replace_node(node, node->left, elsize, free_fn);
        }
    }
    return _balance(node);
}

bool avl_remove(avl_t *avl, void *element, free_fn_t free_fn) {
    node_t *node = _remove(avl->root, element, avl->elsize, avl->comp, free_fn);
    if (node == NOT_FOUND) return false;
    avl->root = node;
    avl->size--;
    return true;
}

size_t avl_get_size(avl_t *avl) { return avl->size; }

// Special struct that describes aditional arguments for the `repass_element`
// function.
struct repass_args {
    elem_fn_t fn;
    void *args;
};

// When iterating through every node, call some function on its value.
static void repass_element(node_t *node, void *repass_args) {
    struct repass_args *args = (struct repass_args *)repass_args;
    args->fn(node->value, args->args);
}

static void _inorder_foreach_node(node_t *node, node_fn_t func, void *args) {
    if (!node) return;
    _inorder_foreach_node(node->left, func, args);
    func(node, args);
    _inorder_foreach_node(node->right, func, args);
}

void avl_inorder_foreach_node(avl_t *avl, node_fn_t func, void *args) {
    _inorder_foreach_node(avl->root, func, args);
}

void avl_inorder_foreach(avl_t *avl, elem_fn_t func, void *args) {
    struct repass_args repass_args = {
        .fn = func,
        .args = args,
    };
    avl_inorder_foreach_node(avl, repass_element, &repass_args);
}

static void _preorder_foreach_node(node_t *node, node_fn_t func, void *args) {
    if (!node) return;
    func(node, args);
    _preorder_foreach_node(node->left, func, args);
    _preorder_foreach_node(node->right, func, args);
}

void avl_preorder_foreach_node(avl_t *avl, node_fn_t func, void *args) {
    _preorder_foreach_node(avl->root, func, args);
}

void avl_preorder_foreach(avl_t *avl, elem_fn_t func, void *args) {
    struct repass_args repass_args = {
        .fn = func,
        .args = args,
    };
    avl_preorder_foreach_node(avl, repass_element, &repass_args);
}

static void _postorder_foreach_node(node_t *node, node_fn_t func, void *args) {
    if (!node) return;
    _postorder_foreach_node(node->left, func, args);
    _postorder_foreach_node(node->right, func, args);
    func(node, args);
}

void avl_postorder_foreach_node(avl_t *avl, node_fn_t func, void *args) {
    _postorder_foreach_node(avl->root, func, args);
}

void avl_postorder_foreach(avl_t *avl, elem_fn_t func, void *args) {
    struct repass_args repass_args = {
        .fn = func,
        .args = args,
    };
    avl_postorder_foreach_node(avl, repass_element, &repass_args);
}

void avl_bfs_foreach_node(avl_t *avl, node_fn_t func, void *args) {
    queue_t *queue = queue_create_with_cap(sizeof(node_t *), avl->size);
    queue_push(queue, &avl->root);

    node_t *curr;
    while (!queue_is_empty(queue)) {
        curr = *(node_t **)queue_pop(queue);
        func(curr, args);
        if (curr->left) queue_push(queue, &curr->left);
        if (curr->right) queue_push(queue, &curr->right);
    }

    queue_delete(queue);
}

void avl_bfs_foreach(avl_t *avl, elem_fn_t func, void *args) {
    struct repass_args repass_args = {
        .fn = func,
        .args = args
    };
    avl_bfs_foreach_node(avl, repass_element, &repass_args);
}

inline int avl_node_height(avl_node_t *node) { return node ? node->height : 0; }
inline void *avl_node_value(avl_node_t *node) { return node->value; }
