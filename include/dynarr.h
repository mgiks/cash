#ifndef DYNARR_H
#define DYNARR_H

#include <stddef.h>

typedef struct {
  void *_data;
  size_t _size;
  size_t _capacity;
  size_t _elem_size;
} DynArr;

DynArr new_dynarr(size_t elem_size);

void push_to_dynarr(DynArr *arr, void *elem);

void pop_from_dynarr(DynArr *arr);

void get_from_dynarr(void *out, DynArr *arr, size_t i);

void free_dynarr(DynArr *arr);

#endif
