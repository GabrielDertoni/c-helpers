#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <llrb_tree.h>

// TODO: remove
#include <stdio.h>

typedef struct _node node_t;

struct _node {
    bool is_red;
    void *value;
    node_t *left;
    node_t *right;
};

struct _llrb_tree {
    node_t *root;
    size_t elsize;
    comp_fn_t comp;
};

llrb_tree_t *llrb_create(size_t elsize, comp_fn_t comp) {
    llrb_tree_t *tree = (llrb_tree_t *)malloc(sizeof(llrb_tree_t));
    tree->root = NULL;
    tree->elsize = elsize;
    tree->comp = comp;
    return tree;
}

static void _delete(node_t *node, free_fn_t free_fn) {
    if (!node) return;
    free_fn(node->value);
    _delete(node->left, free_fn);
    _delete(node->right, free_fn);
    free(node);
}

void llrb_delete(llrb_tree_t *tree, free_fn_t free_fn) {
    if (!tree) return;
    _delete(tree->root, free_fn);
    free(tree);
}

static node_t *_search(node_t *node, void *element, comp_fn_t comp) {
    if (!node) return NULL;
    int cmp = comp(element, node->value);
    if (cmp > 0) return _search(node->right, element, comp);
    if (cmp < 0) return _search(node->left, element, comp);
    return node;
}

void *llrb_search(llrb_tree_t *tree, void *element) {
    node_t *node = _search(tree->root, element, tree->comp);
    return node ? node->value : NULL;
}

static node_t *_create_node(void *element, size_t elsize) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->is_red = true;
    node->left = NULL;
    node->right = NULL;
    node->value = memcpy(malloc(elsize), element, elsize);
    return node;
}

static bool _is_red(node_t *node) {
    if (!node) return false;
    return node->is_red;
}

static node_t *_rotate_right(node_t *node) {
    node_t *new_root = node->left;
    node->left = new_root->right;
    new_root->right = node;
    
    return new_root;
}

static node_t *_rotate_left(node_t *node) {
    node_t *new_root = node->right;
    node->right = new_root->left;
    new_root->left = node;

    return new_root;
}

static void _color_swap(node_t *node_a, node_t *node_b) {
    bool tmp = node_a->is_red;
    node_a->is_red = node_b->is_red;
    node_b->is_red = tmp;
}

static node_t *_insert(node_t *node, void *element, size_t elsize, comp_fn_t comp) {
    if (!node)
        return _create_node(element, elsize);

    node_t *inserted = NULL;
    int cmp = comp(element, node->value);

    // Duplicate keys are not allowed.
    if (!cmp) return NULL;

    // Perform BST-like insertion.
    if (cmp > 0) {
        inserted = _insert(node->right, element, elsize, comp);
        if (!inserted) return NULL;

        node->right = inserted;
    } else {
        inserted = _insert(node->left, element, elsize, comp);
        if (!inserted) return NULL;

        node->left = inserted;
    }

    // Make sure the tree is leaning left
    if (_is_red(node->right) && !_is_red(node->left)) {
        node = _rotate_left(node);
        _color_swap(node, node->left);
    }

    // Rebalance red nodes.
    if (_is_red(node->left) && _is_red(node->left->left)) {
        node = _rotate_right(node);
        _color_swap(node, node->right);
    }

    // If the analogous 2-3 tree node exceeds its limit, promote the node.
    if (_is_red(node->right) && _is_red(node->left)) {
        node->is_red = !node->is_red;
        node->left->is_red = false;
        node->right->is_red = false;
    }
    
    return node;
}

bool llrb_insert(llrb_tree_t *tree, void *element) {
    node_t *node = _insert(tree->root, element, tree->elsize, tree->comp);
    if (node) {
        tree->root = node;
        tree->root->is_red = false;
        return true;
    }
    return false;
}

/*
static node_t *_parent(node_t *root, node_t *node, comp_fn_t comp) {
    if (root == node) return NULL;

    while (root && root->left != node && root->right != node) {
        int cmp = comp(node->value, root->value);
        if (cmp > 0) root = root->left;
        else if (cmp < 0) root = root->right;
        else return NULL; // This should never happen. Implies duplicate keys.
    }
    return root;
}
*/

static node_t *_max(node_t *node) {
    for (; node && node->right; node = node->right);
    return node;
}

void *llrb_max(llrb_tree_t *tree) {
    node_t *node = _max(tree->root);
    return node ? node->value : NULL;
}

static node_t *_min(node_t *node) {
    for (; node && node->left; node = node->left);
    return node;
}

void *llrb_min(llrb_tree_t *tree) {
    node_t *node = _min(tree->root);
    return node ? node->value : NULL;
}

static node_t *_successor(node_t *node, void *element, comp_fn_t comp) {
    if (!node) return NULL;
    node_t *succ = NULL; // The parent node successor.

    int cmp;
    while (node && (cmp = comp(element, node->value)) != 0) {
        if (cmp < 0) {
            succ = node;
            node = node->left;
        } else {
            node = node->right;
        }
    }

    if (!node)
        return NULL;

    if (node->right)
        return _min(node->right); // The child node successor.

    return succ;
}

void *llrb_successor(llrb_tree_t *tree, void *element) {
    node_t *node = _successor(tree->root, element, tree->comp);
    return node ? node->value : NULL;
}

static node_t *_predecessor(node_t *node, void *element, comp_fn_t comp) {
    if (!node) return NULL;
    node_t *pred = NULL; // The parent predecessor.

    int cmp;
    while (node && (cmp = comp(element, node->value)) != 0) {
        if (cmp > 0) {
            pred = node;
            node = node->right;
        } else {
            node = node->left;
        }
    }

    if (!node)
        return NULL;

    if (node->left)
        return _max(node->left);

    return pred;
}

void *llrb_predecessor(llrb_tree_t *tree, void *element) {
    node_t *node = _predecessor(tree->root, element, tree->comp);
    return node ? node->value : NULL;
}

static void _preorder_foreach(node_t *node, foreach_fn_t func, void *args) {
    if (!node) return;
    func(node->value, args);
    _preorder_foreach(node->left, func, args);
    _preorder_foreach(node->right, func, args);
}

void llrb_preorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args) {
    _preorder_foreach(tree->root, func, args);
}

static void _postorder_foreach(node_t *node, foreach_fn_t func, void *args) {
    if (!node) return;
    _postorder_foreach(node->left, func, args);
    _postorder_foreach(node->right, func, args);
    func(node->value, args);
}

void llrb_postorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args) {
    _postorder_foreach(tree->root, func, args);
}

static void _inorder_foreach(node_t *node, foreach_fn_t func, void *args) {
    if (!node) return;
    _inorder_foreach(node->left, func, args);
    func(node->value, args);
    _inorder_foreach(node->right, func, args);
}

void llrb_inorder_foreach(llrb_tree_t *tree, foreach_fn_t func, void *args) {
    _inorder_foreach(tree->root, func, args);
}
