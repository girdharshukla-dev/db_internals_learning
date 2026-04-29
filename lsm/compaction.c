#include "compaction.h"
#include "db.h"

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>

struct kv_pair {
  uint64_t key;
  uint64_t value;
};

int compact_db(struct db_type *db) {
  int fd_a = open(db->sstable_paths[0], O_RDONLY);
  int fd_b = open(db->sstable_paths[1], O_RDONLY);

  char out_path[128];
  snprintf(out_path, sizeof(out_path), "sst_%d.dat", db->next_sst_id++);
  int fd_out = open(out_path, O_CREAT | O_WRONLY, 0644);

  struct kv_pair a, b;
  int have_a, have_b;

  size_t nr = 0;
  while (nr < sizeof(a)) {
    ssize_t n = read(fd_a, (char *)&a + nr, sizeof(a) - nr);
    if (n <= 0)
      break;
    nr += n;
  }
  if (nr == sizeof(a))
    have_a = 1;

  size_t nr = 0;
  while (nr < sizeof(b)) {
    ssize_t n = read(fd_a, (char *)&b + nr, sizeof(b) - nr);
    if (n <= 0)
      break;
    nr += n;
  }
  if (nr == sizeof(b))
    have_b = 1;

  while (have_a || have_b) {
    struct kv_pair winner;
    int advance_a = 0, advance_b = 0;
    if (have_a && have_b) {
      if (a.key < b.key) {
        winner = a;
        advance_a = 1;
      } else if (b.key < a.key) {
        winner = b;
        advance_b = 1;
      } else {
        winner = b;
        advance_a = 1;
        advance_b = 1;
      }
    } else if (have_a) {
      winner = a;
      advance_a = 1;
    } else {
      winner = b;
      advance_b = 1;
    }
    size_t nw = 0;
    while (nw < sizeof(winner)) {
      ssize_t n = write(fd_out, (char *)&winner + nw, sizeof(winner) - nw);
      if (n < 0) {
        close(fd_a);
        close(fd_b);
        close(fd_out);
        return -1;
      }
      nw += n;
    }

    if (advance_a) {
      nr = 0;
      have_a = 0;
      while (nr < sizeof(a)) {
        ssize_t n = read(fd_a, (char *)&a + nr, sizeof(a) - nr);
        if (n <= 0)
          break;
        nr += n;
      }
    }
    if (advance_b) {
      while (nr < sizeof(b)) {
        ssize_t n = read(fd_a, (char *)&b + nr, sizeof(b) - nr);
        if (n <= 0)
          break;
        nr += n;
      }
    }
  }

  fsync(fd_out);
  close(fd_a);
  close(fd_b);
  close(fd_out);
  
  remove(db->sstable_paths[0]);
  remove(db->sstable_paths[1]);
  free(db->sstable_paths[0]);
  db->sstable_paths[0] = strdup(out_path);

  for(size_t i = 2; i < db->next_sst_id; i++){
    db->sstable_paths[i-1] = db->sstable_paths[i];
  }
  return 0;
}
