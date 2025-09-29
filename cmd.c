#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynarr.c"

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

DynArr get_all_cmd_abs_paths() {
    DynArr cmd_dir_names = new_dynarr(sizeof(char *));
    DynArr cmd_abs_paths = new_dynarr(sizeof(char *));

    char *path = getenv("PATH");
    if (!path) {
        fprintf(stderr, "PATH environmental variable not found");
        exit(1);
    }

    char *path_copy = strdup(path);
    char *token = strtok(path_copy, ":");

    while (token) {
        push_to_dynarr(&cmd_dir_names, &token);
        token = strtok(NULL, ":");
    }

    for (size_t i = 0; i < cmd_dir_names._size; ++i) {
        char *cmd_dir_name;
        get_from_dynarr(&cmd_dir_name, &cmd_dir_names, i);

        DIR *d = opendir(cmd_dir_name);
        if (d == NULL) {
            printf("Unable to open directory\n");
            exit(1);
        }

        struct dirent *dir;
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
                continue;
            }

            size_t cmd_abs_path_len = strlen(cmd_dir_name) +
                                      strlen(dir->d_name) +
                                      2;  // Two extra bytes for '/' and '\0'

            char *cmd_abs_path = malloc(cmd_abs_path_len);

            sprintf(cmd_abs_path, "%s/%s", cmd_dir_name, dir->d_name);

            push_to_dynarr(&cmd_abs_paths, &cmd_abs_path);
        }

        closedir(d);
    }

    return cmd_abs_paths;
}

char *extract_cmd_from_abs_path(const char *cmd_abs_path) {
    char *cmd_abs_path_copy = malloc(strlen(cmd_abs_path) + 1);
    strcpy(cmd_abs_path_copy, strdup(cmd_abs_path));

    strrev(cmd_abs_path_copy);

    size_t i = 0;
    while (cmd_abs_path_copy[i] != '/') {
        ++i;
    }

    char *cmd = malloc(i + 1);
    strncpy(cmd, cmd_abs_path_copy, i);
    cmd[i] = '\0';

    strrev(cmd);

    return cmd;
}

char *get_cmd_abs_path(DynArr cmds, char *cmd) {
    const size_t cmds_size = cmds._size;
    char *found = NULL;

    for (size_t i = 0; i < cmds_size; ++i) {
        char *cmd_abs_path;
        get_from_dynarr(&cmd_abs_path, &cmds, i);

        char *cmd_to_compare_to = extract_cmd_from_abs_path(cmd_abs_path);

        if (!strcmp(cmd, cmd_to_compare_to)) {
            found = cmd_abs_path;
            break;
        };
    }

    return found;
}

void execute_cmd(DynArr cmds, char *cmd) {
    char *cmd_abs_path = get_cmd_abs_path(cmds, cmd);

    if (!cmd_abs_path) {
        fprintf(stderr, "cash: %s command not found\n", cmd);
    } else {
        printf("%s\n", cmd_abs_path);
    }

    system(cmd_abs_path);
}
