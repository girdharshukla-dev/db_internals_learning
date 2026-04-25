#include "sstable.h"
#include "memtable.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

int sstable_write(const char *path, struct memtable_type *mt) {
  int fd = open(path, O_CREAT | O_WRONLY, 0644);
  size_t mem_count;
  struct kv_pair *data = memtable_dump(mt, &mem_count);
  size_t total_bytes = mem_count * sizeof(struct kv_pair);

  int written = 0;
  while (written < total_bytes) {
    ssize_t n = write(fd, (char *)data + written, total_bytes - written);
    if (n < 0)
      return -1;
    written += n;
  }
  fsync(fd);
  close(fd);
  // free(data);
  return 0;
}

int sstable_get(const char *path, uint64_t key, uint64_t *value) {
  int fd = open(path, O_RDONLY);
  lseek(fd, 0, SEEK_SET);
  struct kv_pair record;
  int total_to_read = sizeof(struct kv_pair);

  while (1) {
    size_t nr = 0;
    while (nr < total_to_read) {
      ssize_t n = read(fd, (char *)&record + nr, total_to_read - nr);
      if (n <= 0) {
        close(fd);
        return -1; // EOF and error
      }
      nr += n;
    }
    if(record.key == key){
      *value = record.value;
      close(fd);
      return 0;
    }
  }
  close(fd);
  return -1;
}
