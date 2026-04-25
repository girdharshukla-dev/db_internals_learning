#include "memtable.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct memtable_type *memtable_create(size_t capacity) {
  struct memtable_type *mt = malloc(sizeof(struct memtable_type));
  mt->capacity = capacity;
  mt->current_size = 0;
  mt->data = malloc(capacity * sizeof(struct kv_pair));
  return mt;
}

int memtable_put(struct memtable_type *mt, uint64_t key, uint64_t value){

  if(mt->current_size >= mt->capacity){
    return -1;
    // flushing will be done here 
  }

  for(size_t i = 0; i < mt->current_size; i++){
    if(mt->data[i].key == key){
      mt->data[i].value = value;
      return 0;
    }
  }

  size_t i = mt->current_size;
  while(i > 0  && mt->data[i-1].key > key){
    mt->data[i] = mt->data[i-1];
    i--;
  }
  mt->data[i] = (struct kv_pair){key, value};
  mt->current_size++;
  return 0;
}

int memtable_get(struct memtable_type *mt, uint64_t key, uint64_t *value){
  int left = 0;
  int right = mt->current_size - 1;
  while(left <= right){
    int mid = (left + right) / 2;
    if(mt->data[mid].key == key){
      *value = mt->data[mid].value;
      return 0;
    }
    if(mt->data[mid].key < key){
      left = mid + 1;
    }else{
      right = mid - 1;
    }
  }
  return -1;
}

