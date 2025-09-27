#include <stdlib.h>

typedef struct {
    char *_data;
    size_t _size;
} Buffer;

Buffer new_buf(size_t size) { return (Buffer){NULL, size}; };
