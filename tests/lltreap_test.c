#include <stdio.h>
#include <stdlib.h>

#include "test_utils.h"
#include <lltreap.h>

int int_compare(void *a, void *b) {
    return *(int *)a - *(int *)b;
}

struct store_args {
    int index;
    int *arr;
};
void store(void *element, void *store_args) {
    struct store_args *args = (struct store_args *)store_args;
    args->arr[args->index++] = *(int *)element;
}

bool test_lltreap_bst_foreach() {
    UNIMPLEMENTED();
}

bool test_lltreap_postorder_foreach() {
    UNIMPLEMENTED();
}

bool test_lltreap_preorder_foreach() {
    UNIMPLEMENTED();
}

bool test_lltreap_inorder_foreach() {
    srand(42);
    lltreap_t *treap = lltreap_create(sizeof(int), int_compare);

    for (int i = 0; i < 100; i++)
        assert_eq(lltreap_insert(treap, &i), true);

    int elements[100];
    struct store_args args = {
        .index = 0,
        .arr = elements,
    };
    lltreap_inorder_foreach(treap, store, &args);

    assert_eq(args.index, 100);
    for (int i = 0; i < 100; i++)
        assert_eq(elements[i], i);

    lltreap_delete(treap, free);
    return true;
}

bool test_lltreap_remove() {
    srand(42);
    int val;
    lltreap_t *treap = lltreap_create(sizeof(int), int_compare);

    val = 0;
    assert_eq(lltreap_insert(treap, &val), true);
    assert_neq(lltreap_search(treap, &val), NULL);

    val = 1;
    assert_eq(lltreap_insert(treap, &val), true);
    assert_neq(lltreap_search(treap, &val), NULL);

    val = 2;
    assert_eq(lltreap_insert(treap, &val), true);
    assert_neq(lltreap_search(treap, &val), NULL);
    assert_eq(lltreap_remove(treap, &val, free), true);
    assert_eq(lltreap_search(treap, &val), NULL);

    val = 0;
    assert_eq(lltreap_remove(treap, &val, free), true);
    assert_eq(lltreap_search(treap, &val), NULL);
    assert_eq(lltreap_remove(treap, &val, free), false);

    val = 1;
    assert_eq(lltreap_remove(treap, &val, free), true);
    assert_eq(lltreap_search(treap, &val), NULL);

    assert_eq(lltreap_get_size(treap), 0);

    for (int i = 0; i < 30; i++)
        assert_eq(lltreap_insert(treap, &i), true);

    for (int i = 0; i < 30; i++)
        assert_eq(lltreap_remove(treap, &i, free), true);

    assert_eq(lltreap_get_size(treap), 0);

    lltreap_delete(treap, free);
    return true;
}

bool test_lltreap_search() {
    srand(42);
    lltreap_t *treap = lltreap_create(sizeof(int), int_compare);

    for (int i = 0; i < 50; i++) {
        assert_eq(lltreap_insert(treap, &i), true);
        assert_eq(lltreap_get_size(treap), i + 1);
    }

    for (int i = 49; i >= 0; i--)
        assert_neq(lltreap_search(treap, &i), NULL);

    lltreap_delete(treap, free);
    return true;
}

bool test_lltreap_insert() {
    srand(42);
    int val;
    lltreap_t *treap = lltreap_create(sizeof(int), int_compare);

    val = 0;
    assert_eq(lltreap_insert(treap, &val), true);
    assert_eq(lltreap_get_size(treap), 1);

    val = 1;
    assert_eq(lltreap_insert(treap, &val), true);
    assert_eq(lltreap_get_size(treap), 2);

    val = 2;
    assert_eq(lltreap_insert(treap, &val), true);
    assert_eq(lltreap_get_size(treap), 3);
    assert_eq(lltreap_insert(treap, &val), false); // No duplicates allowed.
    assert_eq(lltreap_get_size(treap), 3);

    for (int i = 3; i < 50; i++) {
        assert_eq(lltreap_insert(treap, &i), true);
        assert_eq(lltreap_get_size(treap), i + 1);
    }

    lltreap_delete(treap, free);
    return true;
}

bool test_lltreap_create() {
    lltreap_t *treap = lltreap_create(sizeof(int), int_compare);
    assert_neq(treap, NULL);
    assert_eq(lltreap_get_size(treap), 0);

    lltreap_delete(treap, free);
    return true;
}

int main(void) {
    TEST_SETUP();

    test_fn(test_lltreap_create());
    test_fn(test_lltreap_insert());
    test_fn(test_lltreap_search());
    test_fn(test_lltreap_remove());
    test_fn(test_lltreap_inorder_foreach());
    test_fn(test_lltreap_preorder_foreach());
    test_fn(test_lltreap_postorder_foreach());
    test_fn(test_lltreap_bst_foreach());

    TEST_TEARDOWN();
    return EXIT_SUCCESS;
}
