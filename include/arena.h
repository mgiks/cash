#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct {
  char *buffer;
  size_t currentOffset;
  size_t bufferSize;
} Arena;

void ArenaInit(Arena *arena, size_t buffer_size);

void *ArenaAlloc(Arena *arena, size_t num_of_elems, size_t elem_size,
                 size_t align_size);

void ArenaFree(Arena *arena);

#endif
