#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmd.h"
#include "utils.h"

typedef struct {
    char *_data;
    size_t _size;
} Buffer;

int main() {
    size_t buf_size = 256;
    Buffer buf = (Buffer){NULL, buf_size};

    while (1) {
        printf("> ");
        getline(&buf._data, &buf_size, stdin);

        // Trim newline char
        buf._data[strlen(buf._data) - 1] = '\0';

        trim_leading_space(&buf._data);

        if (strlen(buf._data) < 1) {
            continue;
        }

        execute(buf._data);
    }

    free(buf._data);
    free_cmd_abs_paths();

    return 0;
}
