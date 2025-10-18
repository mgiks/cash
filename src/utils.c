#include "../include/utils.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

char *rel_to_abs_path(char *rel_path, char *cwd) {
    char *abs_path;

    if (rel_path[0] != '.' || rel_path[1] != '.' && rel_path[1] != '/') {
        abs_path = strdup(rel_path);
        return abs_path;
    }

    abs_path = malloc(strlen(rel_path) + strlen(cwd) + 2);
    if (!abs_path) {
        perror("failed to allocate memory");
        exit(1);
    }

    strcpy(abs_path, cwd);
    char *rel_path_first_slash = strchr(rel_path, '/');

    if (rel_path[1] == '.') {
        char *cwd_last_slash = strrchr(cwd, '/');
        abs_path[cwd_last_slash - abs_path] = '\0';
    }

    strcat(abs_path, rel_path_first_slash);

    return abs_path;
}

char *get_home_dir() {
    char *home = getenv("HOME");
    if (home != NULL) {
        return home;
    }

    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        return pw->pw_dir;
    }

    return NULL;
}
