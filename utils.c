#include <stddef.h>
#include <string.h>

void trim_leading_space(char **str) {
    char *str_copy = strdup(*str);
    char c;
    size_t i = 0;

    while ((c = (*str)[i]) == ' ') {
        ++str_copy;
        ++i;
    }

    *str = str_copy;
}
