#include "wal.h"

#include "memtable.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

struct wal_record {
  uint64_t key;
  uint64_t value;
};

struct wal_type *wal_open(const char *path) {
  int fd = open(path, O_CREAT | O_APPEND | O_RDWR, 0644);
  struct wal_type *wal = malloc(sizeof(struct wal_type));
  wal->fd = fd;
  return wal;
}

int wal_append(struct wal_type *wal, uint64_t key, uint64_t value) {
  int fd = wal->fd;
  struct kv_pair wal_record = {key, value};
  size_t total = sizeof(wal_record);
  ssize_t written = 0;
  const char *buf = (const char *)&wal_record;

  while (written < total) {
    ssize_t n = write(fd, buf + written, total - written);
    if (n < 0)
      return -1;
    written += n;
  }
  return 0;
}

int wal_replay(struct wal_type *wal, struct memtable_type *mt) {
  struct wal_record record;
  size_t total = sizeof(record);
  char *buffer = (char *)&record;
  lseek(wal->fd, 0, SEEK_SET);
  while (1) {
    size_t nr = 0;
    while (nr < total) {
      ssize_t n = read(wal->fd, buffer + nr, total - nr);
      if (n == 0) {
        return 0;
      } else if (n < 1)
        return -1;
      nr += n;
    }
    memtable_put(mt, record.key, record.value);
  }
  return 0;
}

int wal_sync(struct wal_type *wal) { return fsync(wal->fd); }

void wal_close(struct wal_type *wal) { close(wal->fd); }
