#include "../include/arena.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void ArenaInit(Arena *arena, uint64_t buffer_size) {
    *arena = (Arena){
        .buffer = malloc(buffer_size),
        .bufferSize = buffer_size,
        .currentOffset = 0,
    };
}

void *ArenaAlloc(Arena *arena, size_t num_of_elems, size_t elem_size,
                 size_t align_size) {
    // Alignment must be a power of two
    if (align_size == 0 || (align_size & (align_size - 1)) != 0) {
        return NULL;
    }

    uintptr_t allocation_size = num_of_elems * elem_size;

    // Overflow check
    if (num_of_elems != 0 && allocation_size / num_of_elems != elem_size) {
        return NULL;
    }

    uintptr_t total_offset =
        (uintptr_t)arena->buffer + (uintptr_t)arena->currentOffset;
    uintptr_t padding = total_offset % align_size;

    total_offset += padding;

    if (total_offset + allocation_size >
        (uintptr_t)arena->buffer + (uintptr_t)arena->bufferSize) {
        return NULL;
    }

    arena->currentOffset += (padding + allocation_size);
    memset((void *)total_offset, 0, allocation_size);
    return (void *)total_offset;
}

void ArenaFree(Arena *arena) {
    free(arena->buffer);
    arena->bufferSize = 0;
    arena->currentOffset = 0;
}
