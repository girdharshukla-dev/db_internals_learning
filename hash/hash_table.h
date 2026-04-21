#ifndef HASH_TABLE
#define HASH_TABLE

#include <stddef.h>
#include <stdint.h>

struct hash_table;

struct hash_table* hash_table_create(size_t initial_size);
void hash_table_destroy(struct hash_table *ht);

int hash_table_put(struct hash_table *ht, uint64_t key, uint64_t value);
int hash_table_get(struct hash_table *ht, uint64_t key, uint64_t *result);
int hash_table_delete(struct hash_table *ht, uint64_t key);


#endif