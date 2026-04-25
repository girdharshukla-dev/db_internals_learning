#include "db.h"
#include "wal.h"
#include "memtable.h"
#include "sstable.h"

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define MEMTABLE_SIZE 10
#define MEMTABLE_THRESHOLD 5
#define MAX_SSTABLE 10

struct db_type *db_open(const char *wal_path) {
  struct wal_type *wal = wal_open(wal_path);
  struct memtable_type *mt = memtable_create(MEMTABLE_SIZE);
  struct db_type *db = malloc(sizeof(struct db_type));
  db->wal = wal;
  db->mt = mt;
  db->next_sst_id = 0;
  db->capacity = MAX_SSTABLE;
  db->sstable_paths = malloc(sizeof(char *) * db->capacity);

  wal_replay(wal, db->mt);
  return db;
}

int db_put(struct db_type *db, uint64_t key, uint64_t value) {
  if (wal_append(db->wal, key, value) == -1)
    return -1;
  wal_sync(db->wal);

  if (db->mt->current_size >= MEMTABLE_THRESHOLD) {
    char path[128];
    snprintf(path, sizeof(path), "sst_%d.dat", db->next_sst_id);
    if (sstable_write(path, db->mt) < 0)
      return -1;
    memtable_clear(db->mt);
    db->sstable_paths[db->next_sst_id] = strdup(path);
    db->next_sst_id++;
  }

  return memtable_put(db->mt, key, value);
}

int db_get(struct db_type *db, uint64_t key, uint64_t *value) {
  if (memtable_get(db->mt, key, value) == 0)
    return 0;
  for (size_t i = 0; i < db->next_sst_id; i++) {
    if (sstable_get(db->sstable_paths[i], key, value) == 0) return 0;
  }
  return -1;
};