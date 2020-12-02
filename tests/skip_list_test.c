#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <colors.h>
#include <tests.h>
#include <skip_list.h>

#define WITH_SKIP_LIST(func) ({ \
    skip_list_t *__list = skip_list_create(compare, strdup(""), 5); \
    func(__list); \
    skip_list_destroy(__list, heap_free); \
})

int compare(char *a, char *b) {
    return strcmp(a, b);
}

void heap_free(char *str) {
    free(str);
}

void empty_list(skip_list_t *l) {
    assert(skip_list_size(l) == 0);
}

void insert_elem(skip_list_t *l) {
    skip_list_insert(l, strdup("Hello world"));
    assert(skip_list_size(l) == 1);
}

void insert_fail(skip_list_t *l) {
    assert(skip_list_insert(l, strdup("Some string")));
    assert(!skip_list_insert(l, strdup("Some string")));
}

void remove_elem(skip_list_t *l) {
    skip_list_insert(l, strdup("Hello world"));
    assert(skip_list_size(l) == 1);

    skip_list_remove(l, "Hello world");
    assert(skip_list_size(l) == 0);
}

void insert_remove_elem(skip_list_t *l) {
    int size = 0;
    for (int i = 0; i < 10; i++) {
        if (i % 2) {
            char *str = (char *)malloc(8 * sizeof(char));
            sprintf(str, "%d", i);
            skip_list_insert(l, str);
            assert(skip_list_size(l) == ++size);
        } else {
            char *str = (char *)malloc(8 * sizeof(char));
            sprintf(str, "%d", i - 1);
            skip_list_remove_free(l, str, heap_free);
            assert(skip_list_size(l) == --size);
            free(str);
        }
    }
}

void search_and_fail(skip_list_t *l) {
    skip_list_insert(l, strdup("Foo"));
    skip_node_t *result = skip_list_find(l, "Bar");
    assert(result == NULL);
}

void search_and_succeed(skip_list_t *l) {
    skip_list_insert(l, strdup("Foo"));
    skip_node_t *result = skip_list_find(l, "Foo");
    assert(result != NULL);
    assert(!strcmp(skip_node_value(result), "Foo"));
}

void fails_when_removing_from_empty(skip_list_t *l) {
    bool removed = skip_list_remove_free(l, "Something", heap_free);
    assert(!removed);
}

void fails_when_removing_not_in_list(skip_list_t *l) {
    skip_list_insert(l, strdup("Something here, idk"));
    bool removed = skip_list_remove_free(l, "Something", heap_free);
    assert(!removed);
}

int main() {
    TEST_SETUP();

    NAMED_TEST("empty_list"                     , WITH_SKIP_LIST(empty_list));
    NAMED_TEST("insert_elem"                    , WITH_SKIP_LIST(insert_elem));
    NAMED_TEST("insert_and_fail"                , WITH_SKIP_LIST(insert_fail));
    NAMED_TEST("remove_elem"                    , WITH_SKIP_LIST(remove_elem));
    NAMED_TEST("insert_remove_elem"             , WITH_SKIP_LIST(insert_remove_elem));
    NAMED_TEST("search_and_fail"                , WITH_SKIP_LIST(search_and_fail));
    NAMED_TEST("search_and_succeed"             , WITH_SKIP_LIST(search_and_succeed));
    NAMED_TEST("fails_when_removing_from_empty" , WITH_SKIP_LIST(fails_when_removing_from_empty));
    NAMED_TEST("fails_when_removing_not_in_list", WITH_SKIP_LIST(fails_when_removing_not_in_list));

    TEST_TEARDOWN();

    return EXIT_SUCCESS;
}
