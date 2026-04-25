#ifndef WAL_H
#define WAL_H

#include <stdint.h>

struct wal_type {
  int fd;
};

struct wal_type *wal_open(const char *path);
int wal_append(struct wal_type *wal, uint64_t key, uint64_t value);
int wal_sync(struct wal_type *wal);

struct memtable_type;
int wal_replay(struct wal_type *wal, struct memtable_type *mt);
void wal_close(struct wal_type *wal);

#endif
