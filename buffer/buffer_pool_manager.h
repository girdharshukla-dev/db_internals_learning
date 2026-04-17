#ifndef BUFFER_POOL_MANAGER_H
#define BUFFER_POOL_MANAGER_H

#define PAGE_SIZE 4096

struct db_frame {
  int page_id;
  int pin_count;
  int is_dirty;
  int ref_bit;
  char data[PAGE_SIZE];
};

struct page_table_entry {
  int frame_id;
  int page_id;
};


struct db_buffer_pool {
  struct db_frame *frames;
  int pool_size;

  struct page_table_entry *page_entry;

  int *free_list;
  int free_count;

  int *lru_list;
  int lru_size;

  // pthread_mutex_t latch;

  int next_page_id;
  int fd;

  int clock_hand;
};

int find_victim(struct db_buffer_pool *bp);
void pt_insert(struct db_buffer_pool *bp, int page_id, int frame_id);
void unpin_page(struct db_buffer_pool *bp, int page_id, int is_dirty);
struct db_buffer_pool *buffer_init(int pool_size, int fd);
struct db_frame *fetch_page(struct db_buffer_pool *bp, int page_id);


#endif
