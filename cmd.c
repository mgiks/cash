#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dynarr.c"
#include "utils.c"

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
    char *last_slash = strrchr(cmd_abs_path, '/');

    if (!last_slash) {
        return strdup(cmd_abs_path);
    }

    return strdup(last_slash + 1);
}

char *get_cmd_abs_path(DynArr cmds, char *cmd) {
    for (size_t i = 0; i < cmds._size; ++i) {
        char *cmd_abs_path;
        get_from_dynarr(&cmd_abs_path, &cmds, i);

        char *cmd_to_compare_to = extract_cmd_from_abs_path(cmd_abs_path);

        char *cmd_dup = strdup(cmd);

        char *cmd_without_args = strtok(cmd_dup, " ");

        if (!strcmp(cmd_without_args, cmd_to_compare_to)) {
            free(cmd_to_compare_to);
            free(cmd_without_args);
            return cmd_abs_path;
        };

        free(cmd_to_compare_to);
        free(cmd_without_args);
    }

    return NULL;
}

char **parse_args(char *input, char *cmd_name) {
    char *input_copy = malloc(strlen(input) + 1);
    strcpy(input_copy, input);

    size_t i = 0;
    while (input_copy[i] != ' ' && input_copy[i] != '\0') {
        ++i;
    }

    char *args_str = input_copy + i;
    if (!*(args_str)) {
        return NULL;
    }

    trim_leading_space(&args_str);

    DynArr args = new_dynarr(sizeof(char *));
    push_to_dynarr(&args, &cmd_name);

    char *arg = strtok(args_str, " ");

    while (arg) {
        push_to_dynarr(&args, &arg);
        arg = strtok(NULL, " ");
    }

    char *null_str = NULL;

    push_to_dynarr(&args, &null_str);

    return args._data;
}

char *parse_cmd(char *input) {
    char *input_copy = malloc(strlen(input) + 1);
    strcpy(input_copy, input);

    char *cmd = strtok(input_copy, " ");
    if (!cmd) {
        return input;
    }

    return cmd;
}

void execute(DynArr cmds, char *input) {
    char *cmd_abs_path = get_cmd_abs_path(cmds, input);
    char *cmd = parse_cmd(input);
    char **args = parse_args(input, cmd);

    if (!cmd_abs_path) {
        fprintf(stderr, "cash: %s command not found\n", input);
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        execv(cmd_abs_path, args);
    } else {
        wait(NULL);
    }

    free(cmd);
}
