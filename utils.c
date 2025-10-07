#include "utils.h"

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
