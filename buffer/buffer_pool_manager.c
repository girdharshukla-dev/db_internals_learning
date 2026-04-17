#include "buffer_pool_manager.h"

#include <stdlib.h>
#include <pthread.h>

struct db_buffer_pool *buffer_init(int pool_size, int fd) {
  struct db_buffer_pool *bp = malloc(sizeof(struct db_buffer_pool));
  bp->pool_size = pool_size;
  bp->fd = fd;
  bp->next_page_id = 0;

  // pthread_mutex_init(&bp->latch, NULL);

  bp->frames = malloc(bp->pool_size * sizeof(struct db_frame));
  for (int i = 0; i < pool_size; i++) {
    bp->frames[i].page_id = -1;
    bp->frames[i].pin_count = 0;
    bp->frames[i].is_dirty = 0;
  }

  bp->page_entry = malloc(bp->pool_size * sizeof(struct page_table_entry));
  for (int i = 0; i < bp->pool_size; i++) {
    bp->page_entry[i].page_id = -1;
  }
  bp->free_list = malloc(pool_size * sizeof(int));
  for (size_t i = 0; i < pool_size; i++) {
    bp->free_list[i] = i;
  }
  bp->free_count = bp->pool_size;
  return bp;
}

int pt_lookup(struct db_buffer_pool *bp, int page_id) {
  for (size_t i = 0; i < bp->pool_size; i++) {
    if (bp->page_entry[i].page_id == page_id) {
      return bp->page_entry[i].frame_id;
    }
  }
  return -1;
}

void pt_insert(struct db_buffer_pool *bp, int page_id, int frame_id) {
  for (size_t i = 0; i < bp->pool_size; i++) {
    if (bp->page_entry[i].page_id == -1) {
      bp->page_entry[i].page_id = page_id;
      bp->page_entry[i].frame_id = frame_id;
      break;
    }
  }
}

struct db_frame *fetch_page(struct db_buffer_pool *bp, int page_id) {
  int frame_id = pt_lookup(bp, page_id);
  if (frame_id != -1) {
    struct db_frame *frame = &bp->frames[frame_id];
    frame->pin_count += 1;
    return frame;
  }

  int new_frame_id;

  if (bp->free_count == 0) {
    int victim_frame_id = find_victim(bp);
    if (victim_frame_id == -1) {
      return NULL;
    }
    if(bp->frames[victim_frame_id].is_dirty == 1){
      //write to disk
    }
    for (size_t j = 0; j < bp->pool_size; j++) {
      if (bp->page_entry[j].frame_id == victim_frame_id) {
        bp->page_entry[j].page_id = -1;
        bp->page_entry[j].frame_id = -1;
      }
    }
    new_frame_id = victim_frame_id;
  }else{
    new_frame_id = bp->free_list[--bp->free_count];
  }

  struct db_frame *frame = &bp->frames[new_frame_id];
  frame->page_id = page_id;
  frame->pin_count = 1;
  frame->is_dirty = 0;

  pt_insert(bp, page_id, new_frame_id);
  return frame;
}

void unpin_page(struct db_buffer_pool *bp, int page_id, int is_dirty) {
  int frame_id = pt_lookup(bp, page_id);
  if (frame_id != -1) {
    bp->frames[frame_id].pin_count -= 1;
    if (is_dirty) {
      bp->frames[frame_id].is_dirty = 1;
    }
  }
}

int find_victim(struct db_buffer_pool *bp) {
  for (size_t i = 0; i < bp->pool_size; i++) {
    if (bp->frames[i].pin_count == 0) {
      return i;
    }
  }
  return -1;
}

