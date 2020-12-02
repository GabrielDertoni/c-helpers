#include <stdio.h>
#include <stdlib.h>

#include "test_utils.h"
#include <llrb_tree.h>

int double_compare(void *a, void *b) {
    double da = *(double *)a;
    double db = *(double *)b;
    if (da > db) return 1;
    if (da < db) return -1;
    return 0;
}

int int_compare(void *a, void *b) {
    return *(int *)a - *(int *)b;
}

void do_nothing(void *_) {}

static int global_index;
void store(void *element, void *arg) {
    int *arr = (int *)arg;
    arr[global_index++] = *(int *)element;
}

bool test_llrb_postorder_foreach() {
    UNIMPLEMENTED();
}

bool test_llrb_preorder_foreach() {
    UNIMPLEMENTED();
}

bool test_llrb_inorder_foreach() {
    llrb_tree_t *tree = llrb_create(sizeof(int), int_compare);
    assert_neq(tree, NULL);

    for (int i = 0; i < 50; i++)
        assert_eq(llrb_insert(tree, &i), true);

    int in_order[50];
    global_index = 0;
    llrb_inorder_foreach(tree, store, in_order);

    assert_eq(global_index, 50);
    for (int i = 0; i < 49; i++)
        assert_le(in_order[i], in_order[i + 1]);
    
    llrb_delete(tree, do_nothing);
    return true;
}

bool test_llrb_successor() {
    llrb_tree_t *tree = llrb_create(sizeof(int), int_compare);
    assert_neq(tree, NULL);

    int val = 1;
    for (int i = 1; i <= 16; i++) {
        assert_eq(llrb_insert(tree, &val), true);
        val *= 2;
    }

    val = 1;
    for (int i = 0; i < 10; i++) {
        assert_eq(*(int *)llrb_successor(tree, &val), val * 2);
        val *= 2;
    }

    val = 0;
    // Cannot find elements not contained in the tree.
    assert_eq(llrb_predecessor(tree, &val), NULL);

    llrb_delete(tree, do_nothing);
    return true;
}

bool test_llrb_predecessor() {
    llrb_tree_t *tree = llrb_create(sizeof(int), int_compare);
    assert_neq(tree, NULL);

    for (int i = 50; i <= 80; i++)
        assert_eq(llrb_insert(tree, &i), true);

    srand(123);
    for (int i = 0; i < 20; i++) {
        int val = rand() % 30 + 51;
        int *result = llrb_predecessor(tree, &val);
        assert_neq(result, NULL);
        assert_eq(*result, val - 1);
    }

    int tmp = 120;
    // Cannot find elements not contained in the tree.
    assert_eq(llrb_predecessor(tree, &tmp), NULL);

    llrb_delete(tree, do_nothing);
    return true;
}

bool test_llrb_min() {
    llrb_tree_t *tree = llrb_create(sizeof(int), int_compare);
    assert_neq(tree, NULL);

    for (int i = 100; i <= 200; i++)
        assert_eq(llrb_insert(tree, &i), true);

    assert_eq(*(int *)llrb_min(tree), 100);

    llrb_delete(tree, do_nothing);
    return true;
}

bool test_llrb_max() {
    llrb_tree_t *tree = llrb_create(sizeof(int), int_compare);
    assert_neq(tree, NULL);

    for (int i = 180; i <= 200; i++)
        assert_eq(llrb_insert(tree, &i), true);

    assert_eq(*(int *)llrb_max(tree), 200);

    llrb_delete(tree, do_nothing);
    return true;
}

bool test_llrb_search() {
    llrb_tree_t *tree = llrb_create(sizeof(int), int_compare);
    assert_neq(tree, NULL);

    for (int i = 0; i < 100; i++)
        assert_eq(llrb_insert(tree, &i), true);

    for (int i = 0; i < 100; i++) {
        int *result = llrb_search(tree, &i);
        assert_neq(result, NULL);
        assert_eq(*result, i);
    }

    llrb_delete(tree, do_nothing);
    return true;
}

bool test_llrb_remove() {
    UNIMPLEMENTED();
}

bool test_llrb_insert() {
    int val;
    llrb_tree_t *tree = llrb_create(sizeof(int), int_compare);
    assert_neq(tree, NULL);

    val = 7;
    assert_eq(llrb_insert(tree, &val), true);

    val = 10;
    assert_eq(llrb_insert(tree, &val), true);

    val = 12;
    assert_eq(llrb_insert(tree, &val), true);

    val = 20;
    assert_eq(llrb_insert(tree, &val), true);

    val = 18;
    assert_eq(llrb_insert(tree, &val), true);
    assert_eq(llrb_insert(tree, &val), false); // No duplicates allowed.

    for (int i = 100; i < 200; i++)
        llrb_insert(tree, &i);

    llrb_delete(tree, do_nothing);
    return true;
}

bool test_llrb_create() {
    llrb_tree_t *tree = llrb_create(sizeof(double), double_compare);
    assert_neq(tree, NULL);

    llrb_delete(tree, do_nothing);
    return true;
}

int main(void) {
    TEST_SETUP();

    test_fn(test_llrb_create());
    test_fn(test_llrb_insert());
    test_fn(test_llrb_remove());
    test_fn(test_llrb_search());
    test_fn(test_llrb_max());
    test_fn(test_llrb_min());
    test_fn(test_llrb_predecessor());
    test_fn(test_llrb_successor());
    test_fn(test_llrb_preorder_foreach());
    test_fn(test_llrb_inorder_foreach());
    test_fn(test_llrb_postorder_foreach());

    TEST_TEARDOWN();
    return EXIT_SUCCESS;
}

