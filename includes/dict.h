#ifndef __DICT_H__
#define __DICT_H__

#include <stdlib.h>
#include <stdbool.h>
#include <utils.h>

typedef struct _dict dict_t;
typedef struct _dict_entry dict_entry_t;
typedef void * dict_value_t;

typedef void (*free_fn_t)(dict_value_t);

dict_t *dict_create();
dict_t *dict_create_sized(size_t size);

dict_entry_t *dict_get_entry(dict_t *dict, char *key);
dict_value_t dict_get(dict_t *dict, char *key);
bool dict_contains(dict_t *dict, char *key);
size_t dict_get_size(dict_t *dict);

bool dict_insert(dict_t *dict, char *key, dict_value_t value);
bool dict_insert_entry(dict_t *dict, dict_entry_t *entry);
bool dict_remove(dict_t *dict, char *key, free_fn_t free_fn);
dict_value_t dict_remove_entry(dict_t *dict, char *key);

void dict_delete(dict_t *dict, free_fn_t free_fn);

#endif
