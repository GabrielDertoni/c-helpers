/**
 * Hashset Module
 *
 * Module that inplements a Hashset as an Abstract Data Type.
 *
 * Inplementation details:
 * The load factor is set to 0.75 by default and de starting table size is 16.
 * To resolve collisions, chaining is used.
 *
 * NOTE: This Hashset implementation only allows for string keys.
 */

#ifndef __HASHSET_H__
#define __HASHSET_H__

#include <stdlib.h>
#include <stdbool.h>

typedef struct _hashset_entry hashset_entry_t;
typedef struct _hashset hashset_t;

/**
 * Creates a hashset with some starting table size.
 *
 * @param size - the starting table size.
 * @return a pointer to the created hashset. [ownership]
 */
hashset_t *hashset_create_sized(size_t size);

/**
 * Creates a hashset using the default starting table size of 16.
 *
 * @return a pointer to the created hashset. [ownership]
 */
hashset_t *hashset_create();

/**
 * Gets the number items in the hashset.
 *
 * @param hashset - the hashset to get the size from. [ref]
 * @return the number of items in the hashset.
 */
size_t hashset_get_size(hashset_t *hashset);

/**
 * Verifies if the hashset contains a certain key.
 *
 * @param hashset - the hashset to search on. [ref]
 * @param key - the key to search for. [ref]
 * @return true if the key is contained in the hashset, false otherwise.
 */
bool hashset_contains(hashset_t *hashset, char *key);

/**
 * Inserts a new key in the hashset if its not a duplicate.
 *
 * @param hashset - the hashset to insert on. [mut ref]
 * @param key - the key to insert. [ref]
 * @return true if the insertion is successfull, false if its a duplicate.
 */
bool hashset_insert(hashset_t *hashset, char *key);

/**
 * Removes a key from the hashset.
 *
 * @param hashset - the hashset to remove from. [mut ref]
 * @param key - the key to remove. [ref]
 * @return true if the key is removed, flase if its not contained in the hashset.
 */
bool hashset_remove(hashset_t *hashset, char *key);

/**
 * Delets the hashset (frees its contents).
 *
 * @param hashset - the hashset to free. [ownership]
 */
void hashset_delete(hashset_t *hashset);

#endif
