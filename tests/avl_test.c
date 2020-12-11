#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "test_utils.h"
#include <avl.h>

static int int_compare(void *a, void *b) {
    return *(int *)a - *(int *)b;
}

struct store_args {
    int index;
    int *arr;
};
static void store(void *element, void *store_args) {
    struct store_args *args = (struct store_args *)store_args;
    args->arr[args->index++] = *(int *)element;
}

bool test_avl_bst_foreach() {
    UNIMPLEMENTED();
}

bool test_avl_postorder_foreach() {
    srand(42);
    int n = 20;
    int *vec = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++)
        vec[i] = rand() % 400;

    avl_t *avl = avl_create(sizeof(int), int_compare);

    for (int i = 0; i < n; i++)
        avl_insert(avl, &vec[i]);

    int elements[n];
    memset(elements, 0, sizeof(elements));
    struct store_args args = {
        .index = 0,
        .arr = elements,
    };
    avl_postorder_foreach(avl, store, &args);

    int min = INT_MAX, max = INT_MIN, log = 1;
    // Calculate ceil(log2(n))
    for (int i = n; i / 2 > 0; log++, i /= 2);
    // Max value should be right near the end.
    for (max = n - 1; max >= n - log && elements[max] < elements[max - 1]; max--);

    min = elements[0] < elements[1] ? 0 : 1;

    // Assert the minimum value found was in fact the minimum, same for max.
    for (int i = 0; i < n; i++) {
        assert_leq(elements[min], elements[i]);
        assert_geq(elements[max], elements[i]);
    }

    avl_delete(avl, NULL);
    return true;
}

bool test_avl_preorder_foreach() {
    srand(42);
    int n = 1000;
    int *vec = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++)
        vec[i] = rand();

    avl_t *avl = avl_create(sizeof(int), int_compare);

    for (int i = 0; i < n; i++)
        avl_insert(avl, &vec[i]);

    int elements[n];
    struct store_args args = {
        .index = 0,
        .arr = elements,
    };
    avl_preorder_foreach(avl, store, &args);
    int min = INT_MAX, max = INT_MIN, log = 1;
    // Calculate ceil(log2(n))
    for (int i = n; i / 2 > 0; log++, i /= 2);
    // Min value should be right near the start.
    for (min = 0; min <= log && elements[min] > elements[min + 1]; min++);

    max = elements[n - 1] > elements[n - 2] ? n - 1 : n - 2;

    // Assert the minimum value found was in fact the minimum, same for max.
    for (int i = 0; i < n; i++) {
        assert_leq(elements[min], elements[i]);
        assert_geq(elements[max], elements[i]);
    }

    avl_delete(avl, NULL);
    return true;
}

bool test_avl_inorder_foreach() {
    srand(42);
    int n = 100;
    avl_t *avl = avl_create(sizeof(int), int_compare);

    for (int i = 0; i < n; i++)
        assert_eq(avl_insert(avl, &i), true);

    int elements[n];
    struct store_args args = {
        .index = 0,
        .arr = elements,
    };
    avl_inorder_foreach(avl, store, &args);

    assert_eq(args.index, n);
    for (int i = 0; i < n; i++)
        assert_eq(elements[i], i);

    avl_delete(avl, NULL);
    return true;
}

bool test_avl_remove() {
    srand(42);
    int val;
    avl_t *avl = avl_create(sizeof(int), int_compare);

    val = 0;
    assert_eq(avl_insert(avl, &val), true);
    assert_neq(avl_search(avl, &val), NULL);

    val = 1;
    assert_eq(avl_insert(avl, &val), true);
    assert_neq(avl_search(avl, &val), NULL);

    val = 2;
    assert_eq(avl_insert(avl, &val), true);
    assert_neq(avl_search(avl, &val), NULL);
    assert_eq(avl_remove(avl, &val, NULL), true);
    assert_eq(avl_search(avl, &val), NULL);

    val = 0;
    assert_eq(avl_remove(avl, &val, NULL), true);
    assert_eq(avl_search(avl, &val), NULL);
    assert_eq(avl_remove(avl, &val, NULL), false);

    val = 1;
    assert_eq(avl_remove(avl, &val, NULL), true);
    assert_eq(avl_search(avl, &val), NULL);

    assert_eq(avl_get_size(avl), 0);

    for (int i = 0; i < 30; i++)
        assert_eq(avl_insert(avl, &i), true);

    for (int i = 0; i < 30; i++)
        assert_eq(avl_remove(avl, &i, NULL), true);

    assert_eq(avl_get_size(avl), 0);

    avl_delete(avl, NULL);
    return true;
}

bool test_avl_search() {
    srand(42);
    avl_t *avl = avl_create(sizeof(int), int_compare);

    for (int i = 0; i < 50; i++) {
        assert_eq(avl_insert(avl, &i), true);
        assert_eq(avl_get_size(avl), i + 1);
    }

    for (int i = 49; i >= 0; i--)
        assert_neq(avl_search(avl, &i), NULL);

    avl_delete(avl, NULL);
    return true;
}

bool test_avl_insert() {
    srand(42);
    int val;
    avl_t *avl = avl_create(sizeof(int), int_compare);

    val = 0;
    assert_eq(avl_insert(avl, &val), true);
    assert_eq(avl_get_size(avl), 1);

    val = 1;
    assert_eq(avl_insert(avl, &val), true);
    assert_eq(avl_get_size(avl), 2);

    val = 2;
    assert_eq(avl_insert(avl, &val), true);
    assert_eq(avl_get_size(avl), 3);
    assert_eq(avl_insert(avl, &val), false); // No duplicates allowed.
    assert_eq(avl_get_size(avl), 3);

    for (int i = 3; i < 50; i++) {
        assert_eq(avl_insert(avl, &i), true);
        assert_eq(avl_get_size(avl), i + 1);
    }

    avl_delete(avl, NULL);
    return true;
}

bool test_avl_create() {
    avl_t *avl = avl_create(sizeof(int), int_compare);
    assert_neq(avl, NULL);
    assert_eq(avl_get_size(avl), 0);

    avl_delete(avl, NULL);
    return true;
}

int main(void) {
    TEST_SETUP();

    test_fn(test_avl_create());
    test_fn(test_avl_insert());
    test_fn(test_avl_search());
    test_fn(test_avl_remove());
    test_fn(test_avl_inorder_foreach());
    test_fn(test_avl_preorder_foreach());
    test_fn(test_avl_postorder_foreach());
    test_fn(test_avl_bst_foreach());

    TEST_TEARDOWN();
    return EXIT_SUCCESS;
}
