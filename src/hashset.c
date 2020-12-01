#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <hashset.h>

#define SET_DEFAULT_SZ 16
#define LOAD_FACTOR 0.75

struct _hashset_entry {
    char *key;
    hashset_entry_t *next;
};

struct _hashset {
    hashset_entry_t **entries;
    size_t max_size;
    size_t size;
};

static size_t hash(char *str);
static hashset_entry_t *mk_entry(char *key, hashset_entry_t *next);

static hashset_entry_t *hashset_get_entry(hashset_t *hashset, char *key) {
    size_t idx = hash(key) % hashset->max_size;

    hashset_entry_t *entry;
    for (entry = hashset->entries[idx];
         entry && strcmp(entry->key, key);
         entry = entry->next);

    return entry;
}

// TODO: Forma de diminuir tamanho além de aumentar.
static void maybe_resize(hashset_t *hashset) {
    if ((float)hashset->size + 1. <= (float)hashset->max_size * LOAD_FACTOR) return;

    hashset_t *new_set = hashset_create_sized(hashset->max_size * 2);
    for (int i = 0; i < hashset->max_size; i++) {
        hashset_entry_t *entry, *tmp;
        for (entry = hashset->entries[i]; entry; entry = tmp) {
            tmp = entry->next;
            size_t idx = hash(entry->key) % new_set->max_size;
            entry->next = new_set->entries[idx];
            new_set->entries[idx] = entry;
        }
    }
    free(hashset->entries);
    hashset->max_size = new_set->max_size;
    hashset->entries = new_set->entries;
    free(new_set);
}

hashset_t *hashset_create_sized(size_t size) {
    hashset_t *hashset = (hashset_t *)malloc(sizeof(hashset_t));
    hashset->max_size = size;
    hashset->size = 0;
    hashset->entries = (hashset_entry_t **)calloc(size, sizeof(hashset_entry_t *));
    return hashset;
}

hashset_t *hashset_create() {
    return hashset_create_sized(SET_DEFAULT_SZ);
}

bool hashset_contains(hashset_t *hashset, char *key) {
    if (hashset_get_entry(hashset, key)) return true;
    return false;
}

size_t hashset_get_size(hashset_t *hashset) { return hashset->size; }

bool hashset_insert_entry(hashset_t *hashset, hashset_entry_t *entry) {
    hashset_entry_t *prev_entry = hashset_get_entry(hashset, entry->key);
    if (prev_entry && !strcmp(prev_entry->key, entry->key)) return false;

    maybe_resize(hashset);
    size_t idx = hash(entry->key) % hashset->max_size;

    entry->next = hashset->entries[idx];
    hashset->entries[idx] = entry;
    hashset->size++;
    return true;
}

bool hashset_insert(hashset_t *hashset, char *key) {
    hashset_entry_t *entry = hashset_get_entry(hashset, key);
    if (entry && !strcmp(entry->key, key)) return false;

    maybe_resize(hashset);
    size_t idx = hash(key) % hashset->max_size;

    entry = mk_entry(key, hashset->entries[idx]);
    hashset->entries[idx] = entry;
    hashset->size++;
    return true;
}

bool hashset_remove(hashset_t *hashset, char *key) {
    size_t idx = hash(key) % hashset->max_size;
    if (!hashset->entries[idx]) return false;

    hashset_entry_t *del, *prev = NULL;
    for (del = hashset->entries[idx];
         del && strcmp(del->key, key);
         prev = del, del = del->next);

    if (!del) return false;
    if (!prev)
        hashset->entries[idx] = del->next;
    else
        prev->next = del->next;

    free(del->key);
    free(del);
    return true;
}

void hashset_delete(hashset_t *hashset) {
    if (!hashset) return;
    hashset_entry_t *entry, *tmp = NULL;
    for (int i = 0; i < hashset->max_size; i++) {
        for (entry = hashset->entries[i]; entry; entry = tmp) {
            free(entry->key);
            tmp = entry->next;
            free(entry);
        }
    }
    free(hashset->entries);
    free(hashset);
}

static hashset_entry_t *mk_entry(char *key, hashset_entry_t *next) {
    hashset_entry_t *entry = (hashset_entry_t *)malloc(sizeof(hashset_entry_t));
    entry->key = key;
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
