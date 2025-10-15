#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>

typedef struct {
  char *buffer;
  uint64_t currentOffset;
  uint64_t bufferSize;
} Arena;

void ArenaInit(Arena *arena, uint64_t buffer_size);

#endif
