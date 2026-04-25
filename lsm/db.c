#include "db.h"
#include "wal.h"
#include "memtable.h"

#include <stdlib.h>
#include <fcntl.h>

#define MEMTABLE_SIZE 10

struct db_type *db_open(const char *wal_path){
  struct wal_type *wal = wal_open(wal_path);
  struct memtable_type *mt = memtable_create(MEMTABLE_SIZE);
  struct db_type *db = malloc(sizeof(struct db_type));
  db->wal = wal;
  db->mt = mt;

  wal_replay(wal, db->mt);
  return db;
}

int db_put(struct db_type *db, uint64_t key, uint64_t value){
  if(wal_append(db->wal, key, value) == -1) return -1;
  wal_sync(db->wal);
  return memtable_put(db->mt, key, value);
}


int db_get(struct db_type *db, uint64_t key, uint64_t *value){
  return memtable_get(db->mt, key, value);
};