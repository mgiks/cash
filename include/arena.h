#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>

typedef struct {
  char *buffer;
  uint64_t currentOffset;
  uint64_t bufferSize;
} Arena;

void ArenaInit(Arena *arena, uint64_t buffer_size);

void *ArenaAlloc(Arena *arena, uint64_t num_of_elems, uint64_t elem_size,
                 uint64_t align_size);

void ArenaFree(Arena *arena);

#endif
