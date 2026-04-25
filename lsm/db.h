#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <stddef.h>

struct db_type {
  struct wal_type *wal;
  struct memtable_type *mt;
  char **sstable_paths;
  uint16_t next_sst_id;
  size_t capacity;
};

struct db_type *db_open(const char *wal_path);
int db_put(struct db_type *db, uint64_t key, uint64_t value);
int db_get(struct db_type *db, uint64_t key, uint64_t *value);
void db_destroy(struct db_type *db);

#endif