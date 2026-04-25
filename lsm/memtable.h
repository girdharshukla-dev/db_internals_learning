#ifndef MEMTABLE_H
#define MEMTABLE_H

#include <stdint.h>
#include <stddef.h>

struct kv_pair{
    uint64_t key;
    uint64_t value;
};

struct memtable_type{
    struct kv_pair *data;
    size_t current_size;
    size_t capacity;
};

struct memtable_type *memtable_create(size_t capacity);
int memtable_put(struct memtable_type *mt, uint64_t key, uint64_t value);
int memtable_get(struct memtable_type *mt, uint64_t key, uint64_t *value);

void memtable_destroy(struct memtable_type *mt);


#endif