#include "hash_table.h"

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

struct entry {
  uint64_t key;
  uint64_t value;
  uint8_t state; // 0->empty, 1->occupied, 2->tomb
};

struct hash_table {
  struct entry *entries;
  size_t capacity;
  size_t count;
};

static size_t next_power_of_2(size_t n) {
  size_t p = 1;
  while (p < n)
    p <<= 1;
  return p;
}

static uint64_t hash(uint64_t alpha) {
  alpha ^= alpha >> 37;
  alpha *= 0xff51afd7ed558ccdULL;
  alpha ^= alpha >> 37;
  alpha *= 0xc4ceb9fe1a85ec53ULL;
  alpha ^= alpha >> 37;
  return alpha;
}

static int hash_table_resize(struct hash_table *ht, size_t new_capacity) {
  if (!ht)
    return 0;
  new_capacity = next_power_of_2(new_capacity);

  struct entry *old_entries = ht->entries;
  size_t old_capacity = ht->capacity;

  struct entry *new_entry = calloc(new_capacity, sizeof(struct entry));
  if(!new_entry) return 0;
  ht->entries = new_entry;
  ht->capacity = new_capacity;
  ht->count = 0;
  size_t mask = ht->capacity - 1;
  for (size_t i = 0; i < old_capacity; i++) {
    if (old_entries[i].state == 1)
      hash_table_put(ht, old_entries[i].key, old_entries[i].value);
  }

  free(old_entries);
  return 1;
}

struct hash_table *hash_table_create(size_t initial_size) {
  struct hash_table *ht = malloc(sizeof(struct hash_table));
  if (!ht)
    return NULL;
  ht->capacity = next_power_of_2(initial_size ? initial_size : 16);
  ht->entries = calloc(ht->capacity, sizeof(struct entry));
  if (!ht->entries) {
    free(ht);
    return NULL;
  }
  ht->count = 0;

  return ht;
}

int hash_table_put(struct hash_table *ht, uint64_t key, uint64_t value) {
  if (!ht)
    return 0;

  if (((ht->count + 1)*100 / ht->capacity) > 70) {
    if (!hash_table_resize(ht, ht->capacity * 2))
      return 0;
  }
  size_t mask = ht->capacity - 1;
  size_t idx = hash(key) & mask;
  size_t first_tomb = SIZE_MAX;

  for (size_t i = 0; i < ht->capacity; i++) {
    struct entry *e = &ht->entries[idx];
    if (e->state == 2 && first_tomb == SIZE_MAX)
      first_tomb = idx;

    if (e->state == 0) {
      size_t index = (first_tomb == SIZE_MAX) ? idx : first_tomb;
      struct entry *in = &ht->entries[index];
      in->key = key;
      in->value = value;
      in->state = 1;
      ht->count++;
      return 1;
    }
    if (e->state == 1 && e->key == key) {
      e->value = value;
      return 1;
    }
    idx = (idx + 1) & mask;
  }
  return 0;
}

int hash_table_get(struct hash_table *ht, uint64_t key, uint64_t *result) {
  size_t mask = ht->capacity - 1;
  size_t idx = hash(key) & mask;

  for (size_t i = 0; i < ht->capacity; i++) {
    if (ht->entries[idx].state == 1 && ht->entries[idx].key == key) {
      *result = ht->entries[idx].value;
      return 1;
    }
    if (ht->entries[idx].state == 0) {
      return 0;
    }
    idx = (idx + 1) & mask;
  }
  return 0;
}

int hash_table_delete(struct hash_table *ht, uint64_t key) {
  size_t mask = ht->capacity - 1;
  size_t idx = hash(key) & mask;

  for (size_t i = 0; i < ht->capacity; i++) {
    if (ht->entries[idx].state == 1 && ht->entries[idx].key == key) {
      ht->entries[idx].state = 2;
      ht->count--;
      return 1;
    }
    if (ht->entries[idx].state == 0) {
      return 0;
    }
    idx = (idx + 1) & mask;
  }
  return 0;
}

void hash_table_destroy(struct hash_table *ht) {
  if (!ht)
    return;

  free(ht->entries);
  free(ht);
}
