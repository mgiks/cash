#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buf.c"
#include "cmd.c"

int main(void) {
    size_t buf_size = 32;
    Buffer buf = new_buf(buf_size);
    DynArr cmd_abs_paths = get_all_cmd_abs_paths();

    while (1) {
        printf(">");
        getline(&buf._data, &buf_size, stdin);

        // Trim newline char
        buf._data[strlen(buf._data) - 1] = '\0';

        trim_leading_space(&buf._data);

        execute(cmd_abs_paths, buf._data);
    }

    free(buf._data);

    return 0;
}
