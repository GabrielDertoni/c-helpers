#include <stdlib.h>
#include <string.h>
#include <queue_bank.h>
#include <lltreap.h>

// Special flag to use when removing an element.
#define NOT_FOUND ((void *)0x1UL)

// Use as shorthand for lltreap_node_t.
typedef lltreap_node_t node_t;

struct _lltreap_node {
    void *value;
    int priority;
    node_t *left;
    node_t *right;
};

struct _lltreap {
    node_t *root;
    size_t size;
    size_t elsize;
    comp_fn_t comp;
};

lltreap_t *lltreap_create(size_t elsize, comp_fn_t comp) {
    lltreap_t *treap = (lltreap_t *)malloc(sizeof(lltreap_t));
    treap->comp = comp;
    treap->elsize = elsize;
    treap->root = NULL;
    treap->size = 0;
    return treap;
}

static void _delete(node_t *node, free_fn_t free_fn) {
    if (!node) return;
    free_fn(node->value);
    _delete(node->left, free_fn);
    _delete(node->right, free_fn);
    free(node);
}

void lltreap_delete(lltreap_t *treap, free_fn_t free_fn) {
    _delete(treap->root, free_fn);
    free(treap);
}

static node_t *_search(node_t *node, void *element, comp_fn_t comp) {
    if (!node) return NULL;
    int cmp = comp(element, node->value);
    if (cmp > 0) return _search(node->right, element, comp);
    if (cmp < 0) return _search(node->left , element, comp);
    return node;
}

void *lltreap_search(lltreap_t *treap, void *element) {
    node_t *node = _search(treap->root, element, treap->comp);
    return node ? node->value : NULL;
}

static node_t *_create_node(void *element, int priority, size_t elsize) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->priority = priority;
    node->value = memcpy(malloc(elsize), element, elsize);
    node->left = NULL;
    node->right = NULL;
    return node;
}

static node_t *_rotate_left(node_t *node) {
    node_t *new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;
    return new_root;
}

static node_t *_rotate_right(node_t *node) {
    node_t *new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;
    return new_root;
}

static node_t *_insert(
    node_t *node,
    void *element,
    int priority,
    size_t elsize,
    comp_fn_t comp
) {
    if (!node) return _create_node(element, priority, elsize);

    int cmp = comp(element, node->value);
    node_t *next;
    if (cmp > 0) {
        next = _insert(node->right, element, priority, elsize, comp);
        if (!next) return NULL;

        node->right = next;

        if (next->priority > node->priority)
            node = _rotate_left(node);

    } else if (cmp < 0) {
        next = _insert(node->left, element, priority, elsize, comp);
        if (!next) return NULL;

        node->left = next;

        if (next->priority > node->priority)
            node = _rotate_right(node);

    } else {
        // Explicitly disallow duplicate keys.
        return NULL;
    }

    return node;
}

bool lltreap_insert(lltreap_t *treap, void *element) {
    return lltreap_insert_with_priority(treap, element, rand());
}

bool lltreap_insert_with_priority(lltreap_t *treap, void *element, int priority) {
    node_t *node = _insert(treap->root, element, priority, treap->elsize, treap->comp);
    if (node) {
        treap->root = node;
        treap->size++;
        return true;
    }
    return false;
}

static node_t *_remove_root(node_t *node) {
    // Verify if it is a leaf or a semi-leaf.
    if (!node->right) return node->left;
    node = _rotate_left(node);
    node->left = _remove_root(node->left);
    return node;
}

static node_t *_remove(node_t *node, void *element, comp_fn_t comp, free_fn_t free_fn) {
    if (!node) return NOT_FOUND;

    node_t *next;
    int cmp = comp(element, node->value);
    if (cmp > 0) {
        next = _remove(node->right, element, comp, free_fn);
        if (next == NOT_FOUND) return NOT_FOUND;

        node->right = next;
    } else if (cmp < 0) {
        next = _remove(node->left , element, comp, free_fn);
        if (next == NOT_FOUND) return NOT_FOUND;

        node->left = next;
    } else {
        next = _remove_root(node);
        free_fn(node->value);
        free(node);
        node = next;
    }

    return node;
}

bool lltreap_remove(lltreap_t *treap, void *element, free_fn_t free_fn) {
    node_t *node = _remove(treap->root, element, treap->comp, free_fn);
    if (node == NOT_FOUND) return false;

    treap->root = node;
    treap->size--;
    return true;
}

size_t lltreap_get_size(lltreap_t *treap) { return treap->size; }

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

void lltreap_inorder_foreach_node(lltreap_t *treap, node_fn_t func, void *args) {
    _inorder_foreach_node(treap->root, func, args);
}

void lltreap_inorder_foreach(lltreap_t *treap, elem_fn_t func, void *args) {
    struct repass_args repass_args = {
        .fn = func,
        .args = args,
    };
    lltreap_inorder_foreach_node(treap, repass_element, &repass_args);
}

static void _preorder_foreach_node(node_t *node, node_fn_t func, void *args) {
    if (!node) return;
    func(node, args);
    _preorder_foreach_node(node->left, func, args);
    _preorder_foreach_node(node->right, func, args);
}

void lltreap_preorder_foreach_node(lltreap_t *treap, node_fn_t func, void *args) {
    _preorder_foreach_node(treap->root, func, args);
}

void lltreap_preorder_foreach(lltreap_t *treap, elem_fn_t func, void *args) {
    struct repass_args repass_args = {
        .fn = func,
        .args = args,
    };
    lltreap_preorder_foreach_node(treap, repass_element, &repass_args);
}

static void _postorder_foreach_node(node_t *node, node_fn_t func, void *args) {
    if (!node) return;
    _postorder_foreach_node(node->left, func, args);
    _postorder_foreach_node(node->right, func, args);
    func(node, args);
}

void lltreap_postorder_foreach_node(lltreap_t *treap, node_fn_t func, void *args) {
    _postorder_foreach_node(treap->root, func, args);
}

void lltreap_postorder_foreach(lltreap_t *treap, elem_fn_t func, void *args) {
    struct repass_args repass_args = {
        .fn = func,
        .args = args,
    };
    lltreap_postorder_foreach_node(treap, repass_element, &repass_args);
}

void lltreap_bfs_foreach_node(lltreap_t *treap, node_fn_t func, void *args) {
    queue_t *queue = queue_create_with_cap(sizeof(node_t *), treap->size);
    queue_push(queue, &treap->root);

    node_t *curr;
    while (!queue_is_empty(queue)) {
        curr = *(node_t **)queue_pop(queue);
        func(curr, args);
        if (curr->left) queue_push(queue, &curr->left);
        if (curr->right) queue_push(queue, &curr->right);
    }

    queue_delete(queue);
}

void lltreap_bfs_foreach(lltreap_t *treap, elem_fn_t func, void *args) {
    struct repass_args repass_args = {
        .fn = func,
        .args = args
    };
    lltreap_bfs_foreach_node(treap, repass_element, &repass_args);
}

inline int lltreap_node_priority(lltreap_node_t *node) { return node->priority; }
inline void *lltreap_node_value(lltreap_node_t *node) { return node->value; }
