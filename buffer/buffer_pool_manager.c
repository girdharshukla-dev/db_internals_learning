#include <stdlib.h>
#include <pthread.h>

#define PAGE_SIZE 4096

struct db_frame {
  int page_id;
  int pin_count;
  int is_dirty;
  char data[PAGE_SIZE];
};

struct page_table_entry{
int frame_id;
int page_id;
};

struct db_buffer_pool {
  struct db_frame *frames;
  int pool_size;

  struct page_table_entry* page_entry;
  
  int *free_list;
  int free_count;

  int *lru_list;
  int lru_size;

  // pthread_mutex_t latch;

  int next_page_id;
  int fd;
};

struct db_buffer_pool* buffer_init(int pool_size, int fd){
  struct db_buffer_pool* bp = malloc(sizeof(struct db_buffer_pool));
  bp->pool_size = pool_size;
  bp->fd = fd;
  bp->next_page_id = 0;

  // pthread_mutex_init(&bp->latch, NULL);

  bp->frames = malloc(sizeof(bp->pool_size * sizeof(struct page_table_entry)));
  for(int i = 0; i < pool_size; i++){
    bp->frames[i].page_id = 0;
    bp->frames[i].pin_count = 0;
    bp->frames[i].is_dirty = 0; 
  }

  bp->page_entry = malloc(sizeof(bp->pool_size * sizeof(struct page_table_entry)));
  for(int i = 0; i < bp->pool_size; i++){
    bp->page_entry[i].page_id = -1;
  }
  bp->free_count = bp->pool_size;
  return bp;
}

int pt_lookup(struct db_buffer_pool *bp, int page_id){
  for(size_t i = 0; i < bp->pool_size; i++){
    if(bp->page_entry[i].page_id == page_id){
      return bp->page_entry[i].frame_id;
    }
  }
  return -1;
}

void pt_insert(struct db_buffer_pool *bp, int page_id, int frame_id){
  
}

int main() {

  return 0;
}