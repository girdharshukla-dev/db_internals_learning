#ifndef SSTABLE_H
#define SSTABLE_H


#include <stdint.h>
#include <stddef.h>

// struct kv_pair {
//   uint64_t key;
//   uint64_t value;
// };

struct memtable_type;
int sstable_write(const char *path, struct memtable_type *mt);
int sstable_get(const char *path, uint64_t key, uint64_t *value);

#endif
