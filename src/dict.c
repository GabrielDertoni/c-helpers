#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <dict.h>

#define DICT_DEFAULT_SZ 16
#define LOAD_FACTOR 0.75

struct _dict_entry {
    char *key;
    dict_value_t value;
    dict_entry_t *next;
};

struct _dict {
    dict_entry_t **entries;
    size_t max_size;
    size_t size;
};

static size_t hash(char *str);
static dict_entry_t *mk_entry(char *key, dict_value_t value, dict_entry_t *next);

// TODO: Forma de diminuir tamanho além de aumentar.
static void maybe_resize(dict_t *dict) {
    if ((float)dict->size + 1. <= (float)dict->max_size * LOAD_FACTOR) return;

    dict_t *new_dict = dict_create_sized(dict->max_size * 2);
    for (int i = 0; i < dict->max_size; i++) {
        dict_entry_t *entry, *tmp;
        for (entry = dict->entries[i]; entry; entry = tmp) {
            tmp = entry->next;
            size_t idx = hash(entry->key) % new_dict->max_size;
            entry->next = new_dict->entries[idx];
            new_dict->entries[idx] = entry;
        }
    }
    free(dict->entries);
    dict->max_size = new_dict->max_size;
    dict->entries = new_dict->entries;
    free(new_dict);
}

dict_t *dict_create_sized(size_t size) {
    dict_t *dict = (dict_t *)malloc(sizeof(dict_t));
    dict->max_size = size;
    dict->size = 0;
    dict->entries = (dict_entry_t **)calloc(size, sizeof(dict_entry_t *));
    return dict;
}

dict_t *dict_create() {
    return dict_create_sized(DICT_DEFAULT_SZ);
}

dict_entry_t *dict_get_entry(dict_t *dict, char *key) {
    size_t idx = hash(key) % dict->max_size;

    dict_entry_t *entry;
    for (entry = dict->entries[idx];
         entry && strcmp(entry->key, key);
         entry = entry->next);

    return entry;
}

dict_value_t dict_get(dict_t *dict, char *key) {
    dict_entry_t *entry = dict_get_entry(dict, key);
    if (entry) return entry->value;
    return NULL;
}

bool dict_contains(dict_t *dict, char *key) {
    if (dict_get(dict, key)) return true;
    return false;
}

size_t dict_get_size(dict_t *dict) { return dict->size; }

bool dict_insert_entry(dict_t *dict, dict_entry_t *entry) {
    dict_entry_t *prev_entry = dict_get_entry(dict, entry->key);
    if (prev_entry && !strcmp(prev_entry->key, entry->key)) return false;

    maybe_resize(dict);
    size_t idx = hash(entry->key) % dict->max_size;

    entry->next = dict->entries[idx];
    dict->entries[idx] = entry;
    dict->size++;
    return true;
}

bool dict_insert(dict_t *dict, char *key, dict_value_t value) {
    dict_entry_t *entry = dict_get_entry(dict, key);
    if (entry && !strcmp(entry->key, key)) return false;

    maybe_resize(dict);
    size_t idx = hash(key) % dict->max_size;

    entry = mk_entry(strdup(key), value, dict->entries[idx]);
    dict->entries[idx] = entry;
    dict->size++;
    return true;
}

dict_value_t dict_remove_entry(dict_t *dict, char *key) {
    size_t idx = hash(key) % dict->max_size;
    if (!dict->entries[idx]) return NULL;

    dict_entry_t *del, *prev = NULL;
    for (del = dict->entries[idx];
         del && strcmp(del->key, key);
         prev = del, del = del->next);

    if (!del) return NULL;
    if (!prev)
        dict->entries[idx] = del->next;
    else
        prev->next = del->next;

    dict_value_t value = del->value;
    free(del->key);
    free(del);
    return value;
}

bool dict_remove(dict_t *dict, char *key, free_fn_t free_fn) {
    dict_value_t value;
    if (!(value = dict_remove_entry(dict, key))) return false;
    free_fn(value);
    return true;
}

void dict_delete(dict_t *dict, free_fn_t free_fn) {
    if (!dict) return;
    dict_entry_t *entry, *tmp = NULL;
    for (int i = 0; i < dict->max_size; i++) {
        for (entry = dict->entries[i]; entry; entry = tmp) {
            free(entry->key);
            free_fn(entry->value);
            tmp = entry->next;
            free(entry);
        }
    }
    free(dict->entries);
    free(dict);
}

static dict_entry_t *mk_entry(char *key, dict_value_t value, dict_entry_t *next) {
    dict_entry_t *entry = (dict_entry_t *)malloc(sizeof(dict_entry_t));
    entry->key = key;
    entry->value = value;
    entry->next = next;
    return entry;
}

// Source: http://www.cse.yorku.ca/~oz/hash.html
static size_t hash(char *str) {
    size_t hash = 0;
    int c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}
