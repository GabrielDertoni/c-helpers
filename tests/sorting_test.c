#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "test_utils.h"
#include <sorting.h>

int int_compare(void *a, void *b) {
    return *(int *)a - *(int *)b;
}

uint int_to_uint(void *a, void *args) {
    return *(int *)a - INT_MIN;
}

bool check_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++)
        assert_leq(arr[i], arr[i + 1]);

    return true;
}

int *create_arr(int n) {
    int *arr = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        arr[i] = rand();

    return arr;
}

int *create_arr_mod(int n, int mod) {
    int *arr = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        arr[i] = rand() % mod;

    return arr;
}

void bench_tim_sort(int *vec, int n) {
    tim_sort_with(vec, n, sizeof(int), int_compare);
}

void bench_quick_sort(int *vec, int n) {
    quick_sort_with(vec, n, sizeof(int), int_compare);
}

bool test_tim_sort() {
    srand(42);
    int n = 1000;
    int *vec = create_arr(n);

    tim_sort_with(vec, n, sizeof(int), int_compare);

    assert_eq(check_sort(vec, n), true);

    free(vec);
    return true;
}

bool test_binary_insertion_sort() {
    srand(42);
    int n = 1000;
    int *vec = create_arr(n);

    binary_insertion_sort_with(vec, n, sizeof(int), int_compare);
    assert_eq(check_sort(vec, n), true);

    free(vec);
    return true;
}

bool test_upper_bound() {
    srand(42);
    int n = 1000;
    int *vec = create_arr_mod(n, 2 * n);
    quick_sort_with(vec, n, sizeof(int), int_compare);

    for (int i = 0; i < 2 * n; i++) {
        int *res = upper_bound(&i, vec, n, sizeof(int), int_compare);
        if (res - vec < n) assert_le(i, vec[res - vec]);
        if (res - vec > 0) assert_geq(i, vec[res - vec - 1]);
    }

    free(vec);
    return true;
}

bool test_lower_bound() {
    srand(42);
    int n = 1000;
    int *vec = create_arr_mod(n, 2 * n);
    quick_sort_with(vec, n, sizeof(int), int_compare);

    for (int i = 0; i < 2 * n; i++) {
        int *res = lower_bound(&i, vec, n, sizeof(int), int_compare);
        if (res - vec < n) assert_leq(i, vec[res - vec]);
        if (res - vec > 0) assert_ge(i, vec[res - vec - 1]);
    }

    free(vec);
    return true;
}

bool test_radix_sort() {
    srand(42);
    int n = 1000;
    int *vec = create_arr(n);

    radix256_sort_with(vec, n, sizeof(int), int_to_uint, NULL);
    assert_eq(check_sort(vec, n), true);

    return true;
}

bool test_merge_sort() {
    srand(42);
    int n = 1000;
    int *vec = create_arr(n);

    merge_sort_with(vec, n, sizeof(int), int_compare);
    assert_eq(check_sort(vec, n), true);

    return true;
}

bool test_quick_sort() {
    srand(42);
    int n = 1000;
    int *vec = create_arr(n);

    quick_sort_with(vec, n, sizeof(int), int_compare);
    assert_eq(check_sort(vec, n), true);

    return true;
}

bool test_heap_sort() {
    srand(42);
    int n = 1000;
    int *vec = create_arr(n);

    heap_sort_with(vec, n, sizeof(int), int_compare);
    assert_eq(check_sort(vec, n), true);

    return true;
}

bool test_insertion_sort() {
    srand(42);
    int n = 500;
    int *vec = create_arr(n);

    insertion_sort_with(vec, n, sizeof(int), int_compare);
    assert_eq(check_sort(vec, n), true);

    return true;
}

int main(void) {
    TEST_SETUP();

    test_fn(test_insertion_sort());
    test_fn(test_heap_sort());
    test_fn(test_quick_sort());
    test_fn(test_merge_sort());
    test_fn(test_radix_sort());
    test_fn(test_lower_bound());
    test_fn(test_upper_bound());
    test_fn(test_binary_insertion_sort());
    test_fn(test_tim_sort());

    int n = 100000;
    int *vec = create_arr(n);
    int *to_sort = malloc(n * sizeof(int));

    memcpy(to_sort, vec, n * sizeof(int));
    bench(bench_quick_sort(to_sort, n));

    memcpy(to_sort, vec, n * sizeof(int));
    bench(bench_tim_sort(to_sort, n));

    free(vec);
    free(to_sort);

    TEST_TEARDOWN();
    return EXIT_SUCCESS;
}
