#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/cmd.h"
#include "../include/utils.h"

typedef struct {
    char *_data;
    size_t _size;
} Buffer;

int main() {
    size_t buf_size = 256;
    Buffer buf = (Buffer){NULL, buf_size};

    char *home_dir = getenv("HOME");
    chdir(home_dir);

    long path_max = sysconf(_PC_PATH_MAX);
    if (path_max < 0) {
        fprintf(stderr, "cash: unable to find PATH_MAX constant");
        return 1;
    }

    size_t path_size = path_max + 1;
    char *cwd = malloc(path_size);
    if (!cwd) {
        perror("malloc");
        return 1;
    }

    while (1) {
        if (getcwd(cwd, path_size) == NULL) {
            fprintf(stderr, "cash: unable to determine CWD: %s\n",
                    strerror(errno));
        };

        if (!strcmp(home_dir, cwd)) {
            printf("~> ");
        } else {
            printf("%s> ", cwd);
        }

        getline(&buf._data, &buf_size, stdin);

        // Trim newline char
        buf._data[strlen(buf._data) - 1] = '\0';

        trim_leading_space(&buf._data);

        if (strlen(buf._data) < 1) {
            continue;
        }

        execute(buf._data, cwd);
    }

    free(cwd);
    free(buf._data);
    free_cmd_abs_paths();

    return 0;
}
