#include "../include/utils.h"

#include <string.h>

void strrev(char *s) {
    size_t l = 0, r = strlen(s) - 1;
    char c;

    while (l < r) {
        c = s[l];
        s[l] = s[r];
        s[r] = c;

        ++l;
        --r;
    }
}

void trim_leading_space(char **str) {
    char *trimmed_str = *str;

    while (*trimmed_str == ' ') {
        ++trimmed_str;
    }

    memmove(*str, trimmed_str, strlen(trimmed_str) + 1);
}

char *trim_path(char *str) {
    char *first_slash = strchr(str, '/');
    char *last_slash = strrchr(str, '/');

    char *trimmed_path;
    if (first_slash != last_slash) {
        trimmed_path = strdup(last_slash + 1);
    } else {
        trimmed_path = strdup(str);
    }

    return trimmed_path;
}
