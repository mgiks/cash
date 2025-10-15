#include "../include/arena.h"

#include <stdint.h>
#include <stdlib.h>

void ArenaInit(Arena *arena, uint64_t buffer_size) {
    *arena = (Arena){
        .buffer = malloc(buffer_size),
        .bufferSize = buffer_size,
        .currentOffset = 0,
    };
}
