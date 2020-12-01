#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <skip_list.h>

struct _skip_node {
    elem_t value;
    skip_node_t *next;
    skip_node_t *below;
};

struct _skip_list {
    skip_node_t *head;
    comp_fn_t comp;
    uint size;
    uint layer_max;
    uint height;
};

/* ============ SKIP NODE ============ */

skip_node_t *skip_node_create(elem_t elem, skip_node_t *next, skip_node_t *below) {
    skip_node_t *node = (skip_node_t *)malloc(sizeof(skip_node_t));
    node->value = elem;
    node->next = next;
    node->below = below;
    return node;
}

/* GETTERS */

skip_node_t *skip_node_next(skip_node_t *node) { return node->next; }
skip_node_t *skip_node_below(skip_node_t *node) { return node->below; }
elem_t skip_node_value(skip_node_t *node) { return node->value; }

/* METHODS */

skip_node_t *skip_node_insert(skip_node_t *head, elem_t elem, comp_fn_t comp) {
    skip_node_t *ptr;
    for (ptr = head; ptr->next && comp(ptr->next->value, elem) <= 0; ptr = ptr->next);

    skip_node_t *below = NULL;
    // If there is a node below and a node has not been inserted below and
    // the coin flip failed, do not add a node to this layer.
    if (ptr->below && !((below = skip_node_insert(ptr->below, elem, comp)) && rand() % 2))
        return NULL;

    return ptr->next = skip_node_create(elem, ptr->next, below);
}

// Will return a node forms a linked list pointers.
skip_node_t *skip_node_remove(skip_node_t *head, elem_t elem, comp_fn_t comp) {
    skip_node_t *ptr, *removed = NULL;
    for (ptr = head; ptr->next && comp(ptr->next->value, elem) < 0; ptr = ptr->next);

    if (ptr->below)
        removed = skip_node_remove(ptr->below, elem, comp);

    if (ptr->next && comp(ptr->next->value, elem) == 0) {
        removed = ptr->next;
        ptr->next = removed->next;
        removed->next = removed->below;
        removed->below = NULL;
    }

    return removed;
}

elem_t skip_node_destroy(skip_node_t *node) {
    elem_t elem = node->value;
    free(node);
    return elem;
}

/* ============ SKIP LIST METHODS ============ */

skip_list_t *skip_list_create(uint layer_max, comp_fn_t comp) {
    srand(time(NULL));
    skip_list_t *skip_list = (skip_list_t *)malloc(sizeof(skip_list_t));
    skip_list->head = skip_node_create(ELEM_MIN, NULL, NULL);
    skip_list->comp = comp;
    skip_list->size = 0;
    skip_list->layer_max = layer_max;
    skip_list->height = 1;
    return skip_list;
}

/* GETTERS */

skip_node_t *skip_list_head(skip_list_t *l) { return l->head; }
bool skip_list_is_empty(skip_list_t *l) { return l->size == 0; }
uint skip_list_size(skip_list_t *l) { return l->size; }

/* METHODS */

skip_node_t *skip_list_find(skip_list_t *l, elem_t elem) {
    int cmp;
    skip_node_t *ptr;
    for (ptr = l->head; ptr->below && cmp; ptr = ptr->below)
        for (; ptr->next && (cmp = l->comp(ptr->next->value, elem)) < 0; ptr = ptr->next);

    if (!ptr->next || l->comp(ptr->next->value, elem) != 0) return NULL;
    return ptr->next;
}

void skip_list_insert(skip_list_t *l, elem_t elem) {
    skip_node_t *below;
    if ((below = skip_node_insert(l->head, elem, l->comp)) && rand() % 2 && l->height < l->layer_max) {
        skip_node_t *top = skip_node_create(elem, NULL, below);
        l->head = skip_node_create(l->head->value, top, l->head);
        l->height++;
    }

    l->size++;
    return;
}

skip_node_t *skip_list_remove(skip_list_t *l, elem_t elem) {
    l->size--;
    skip_node_t *removed = skip_node_remove(l->head, elem, l->comp);
    if (!l->head->next && l->head->below) {
        skip_node_t *below = l->head->below;
        free(l->head);
        l->head = below;
    }
    return removed;
}

bool skip_list_remove_free(skip_list_t *l, elem_t elem, free_fn_t free_func) {
    skip_node_t *ptr = skip_list_remove(l, elem);
    if (ptr == NULL) return false;
    // All values are the same, so only free them once!
    free_func(ptr->value);
    while (ptr) {
        skip_node_t *tmp = ptr->next;
        free(ptr);
        ptr = tmp;
    }
    return true;
}

void skip_list_destroy(skip_list_t *l, free_fn_t free_func) {
    for (skip_node_t *layer = l->head; layer;) {
        skip_node_t *layer_below = layer->below;
        for (skip_node_t *ptr = layer; ptr;) {
            skip_node_t *ptr_next = ptr->next;
            elem_t elem = skip_node_destroy(ptr);
            // Only free elements in the bottom layer, since it is garanteed to
            // have all elements.
            if (!layer_below)
                free_func(elem);

            ptr = ptr_next;
        }
        layer = layer_below;
    }
    free(l);
}

int skip_list_compare(skip_list_t *l, skip_node_t *a, skip_node_t *b) {
    return l->comp(a->value, b->value);
}

/* ============ HELPERS ============ */

void heap_free(elem_t elem) {
    free(elem);
}

int compare(elem_t a, elem_t b) {
    return strcmp(a, b);
}

/*
void stack_free(elem_t elem) {
    return;
}

int compare(elem_t a, elem_t b) {
    return a > b ? 1 : (a < b ? -1 : 0);
}
*/




