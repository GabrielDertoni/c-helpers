#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "test_utils.h"
#include <colors.h>
#include <dict.h>

void test_dict_benchmark(size_t n_inputs) {
    dict_t *dict = dict_create();

    char **keys = (char **)malloc(n_inputs * sizeof(char *));
    char **values = (char **)malloc(n_inputs * sizeof(char *));
    srand(57);
    for (int i = 0; i < n_inputs; i++) {
        int len = rand() % 256 + 10;
        char *key = (char *)malloc((len + 1) * sizeof(char));
        for (int j = 0; j < len; j++) key[j] = 'A' + rand() % 57;
        key[len] = '\0';

        len = rand() % 512 + 10;
        char *value = (char *)malloc((len + 1) * sizeof(char));
        for (int j = 0; j < len; j++) value[j] = 'A' + rand() % 57;
        value[len] = '\0';

        keys[i] = key;
        values[i] = value;
    }

    clock_t start_time = clock();
    for (int i = 0; i < n_inputs; i++)
        dict_insert(dict, keys[i], values[i]);

    printf(CYAN "Insertion benchmark took %lf milliseconds" RESET "\n", (double)(clock() - start_time) / (double)(CLOCKS_PER_SEC / 1000));

    for (int i = 0; i < n_inputs; i++)
        free(keys[i]);

    dict_delete(dict, free);
}

bool test_dict_remove() {
    dict_t *dict = dict_create();
    assert_neq(dict, NULL);

    // Cant remove something that isn't there.
    assert_eq(dict_remove(dict, "Hello", free), false);
    assert_eq(dict_insert(dict, "Hello", strdup("World")), true);
    assert_eq(dict_remove(dict, "Hello", free), true);

    // Removed is no longer accessible.
    assert_eq(dict_get(dict, "Hello"), NULL);

    dict_delete(dict, free);
    return true;
}

bool test_dict_insert() {
    dict_t *dict = dict_create();
    assert_neq(dict, NULL);

    assert_eq(dict_insert(dict, "Hello", strdup("World")), true);
    assert_eq(strcmp(dict_get(dict, "Hello"), "World"), 0);

    assert_eq(dict_insert(dict, "Key", strdup("Value")), true);
    assert_eq(strcmp(dict_get(dict, "Key"), "Value"), 0);

    assert_eq(dict_insert(dict, "Other key here", strdup("Aaannd other value")), true);
    assert_eq(dict_get(dict, "Other"), NULL);

    // No duplicates.
    char *msg = strdup("Something else.");
    assert_eq(dict_insert(dict, "Hello", msg), false);
    free(msg);

    // Lots of inserts.
    srand(42);
    for (int i = 0; i < 1000; i++) {
        int len = rand() % 256 + 10;
        char *key = (char *)malloc((len + 1) * sizeof(char));
        for (int j = 0; j < len; j++) key[j] = 'A' + rand() % 57;
        key[len] = '\0';

        len = rand() % 256 + 10;
        char *value = (char *)malloc((len + 1) * sizeof(char));
        for (int j = 0; j < len; j++) value[j] = 'A' + rand() % 57;
        value[len] = '\0';

        assert_eq(dict_insert(dict, key, value), true);
        assert_eq(strcmp(dict_get(dict, key), value), 0);
        assert_eq(dict_get_size(dict), 4 + i);
        free(key);
    }
    // Even after many inserts, old data remains present.
    assert_eq(strcmp(dict_get(dict, "Key"), "Value"), 0);
    assert_eq(strcmp(dict_get(dict, "Hello"), "World"), 0);

    dict_delete(dict, free);
    return true;
}

bool test_dict_get() {
    dict_t *dict = dict_create();
    assert_neq(dict, NULL);

    assert_eq(dict_get(dict, "Hello"), NULL);

    assert_eq(dict_insert(dict, "Hello", strdup("World")), true);
    assert_eq(strcmp(dict_get(dict, "Hello"), "World"), 0);

    dict_delete(dict, free);
    return true;
}

bool test_dict_create() {
    dict_t *dict = dict_create();
    assert_neq(dict, NULL);
    assert_eq(dict_get_size(dict), 0);

    dict_delete(dict, free);
    return true;
}

int main(int argc, char *argv[]) {
    TEST_SETUP();

    test_fn(test_dict_create());
    test_fn(test_dict_get());
    test_fn(test_dict_insert());
    test_fn(test_dict_remove());

    test_dict_benchmark(10000);

    TEST_TEARDOWN();
    return EXIT_SUCCESS;
}

