#include "../include/cmd.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/dynarr.h"
#include "../include/utils.h"

static DynArr cmd_abs_paths;
static char *prev_dir;

static void init_cmd_abs_paths() {
    if (cmd_abs_paths._size > 0) return;

    DynArr dir_names = new_dynarr(sizeof(char *));
    DynArr abs_paths = new_dynarr(sizeof(char *));

    char *path = getenv("PATH");
    if (!path) {
        fprintf(stderr, "PATH environmental variable not found");
        exit(1);
    }

    char *path_copy = strdup(path);
    char *token = strtok(path_copy, ":");

    while (token) {
        push_to_dynarr(&dir_names, &token);
        token = strtok(NULL, ":");
    }

    for (size_t i = 0; i < dir_names._size; ++i) {
        char *cmd_dir_name;
        get_from_dynarr(&cmd_dir_name, &dir_names, i);

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

            push_to_dynarr(&abs_paths, &cmd_abs_path);
        }

        closedir(d);
    }

    free_dynarr(&dir_names);
    free(path_copy);

    cmd_abs_paths = abs_paths;
}

static char *extract_cmd_name_from_abs_path(char *cmd_abs_path) {
    char *last_slash = strrchr(cmd_abs_path, '/');

    if (!last_slash) {
        return strdup(cmd_abs_path);
    }

    return strdup(last_slash + 1);
}

static char *find_cmd_abs_path(const char *cmd_name) {
    for (size_t i = 0; i < cmd_abs_paths._size; ++i) {
        char *cmd_abs_path;
        get_from_dynarr(&cmd_abs_path, &cmd_abs_paths, i);

        char *cmd_name_to_cmp = extract_cmd_name_from_abs_path(cmd_abs_path);
        if (!strcmp(cmd_name, cmd_name_to_cmp)) {
            free(cmd_name_to_cmp);
            return cmd_abs_path;
        };

        free(cmd_name_to_cmp);
    }

    return NULL;
}

static char *parse_cmd_name(char *input) {
    char *input_copy = malloc(strlen(input) + 1);
    strcpy(input_copy, input);

    char *cmd = strtok(input_copy, " ");
    if (!cmd) {
        return input;
    }

    return cmd;
}

static DynArr parse_args(char *input, long arg_arr_len) {
    char *input_copy = malloc(strlen(input) + 1);
    strcpy(input_copy, input);

    size_t i = 0;
    while (input_copy[i] != ' ' && input_copy[i] != '\0') {
        ++i;
    }

    char *args_str = input_copy + i;
    trim_leading_space(&args_str);

    DynArr args = new_dynarr(sizeof(char *));

    char *cmd_name = parse_cmd_name(input);
    push_to_dynarr(&args, &cmd_name);

    char *null_str = NULL;
    if (strlen(args_str) < 1) {
        push_to_dynarr(&args, &null_str);

        free(input_copy);
        return args;
    }

    size_t arg_i = 1;
    char *arg = strtok(args_str, " ");
    while (arg) {
        char *arg_copy = strdup(arg);
        push_to_dynarr(&args, &arg_copy);
        arg = strtok(NULL, " ");
    }

    push_to_dynarr(&args, &null_str);

    free(input_copy);

    return args;
}

static void free_args_arr(char **args_arr, long args_arr_len) {
    for (int i = 0; i < args_arr_len; ++i) {
        free(args_arr[i]);
    }
    free(args_arr);
}

void execute(char *input, char *cwd) {
    init_cmd_abs_paths();

    char *cmd_name = parse_cmd_name(input);
    char *cmd_abs_path = find_cmd_abs_path(cmd_name);

    long arg_max = sysconf(_SC_ARG_MAX);
    if (arg_max < 0) {
        fprintf(stderr, "cash: unable to find ARG_MAX constant");
        exit(1);
    }

    DynArr args = parse_args(input, arg_max);

    if (cmd_name[0] == '/') {
        cmd_abs_path = cmd_name;
    }

    if (cmd_name[0] == '.') {
        cmd_abs_path = rel_to_abs_path(cmd_name, cwd);
    }

    if (!strcmp(cmd_name, "exit")) {
        // Is set to -1 because the args list always starts with the cmd's name
        size_t args_len = -1;
        char **arg = args._data;

        while ((*arg) != NULL) {
            ++args_len;
            ++arg;
        }

        if (args_len > 1) {
            fprintf(stderr, "cash: exit: too many arguments\n");

            free(cmd_name);
            free_args_arr(args._data, args._size);

            return;
        }

        if (args_len == 1) {
            char *end, *arg;
            get_from_dynarr(&arg, &args, 1);

            long exit_val = strtol(arg, &end, 10);

            // At index 1 due to args list always starting with the cmd's name
            if (end == arg || *end != '\0' || errno == ERANGE) {
                fprintf(stderr, "cash: exit: %s: numeric argument required\n",
                        arg);

                free(cmd_name);
                free_args_arr(args._data, args._size);

                return;
            }

            free(cmd_name);
            free_args_arr(args._data, args._size);

            exit(exit_val);
        }

        free(cmd_name);
        free_args_arr(args._data, args._size);
        free_cmd_abs_paths();

        exit(0);
    }

    if (!strcmp(cmd_name, "cd")) {
        // Is set to -1 because the args list always starts with the cmd's name
        size_t args_len = -1;
        char **arg = args._data;

        while ((*arg) != NULL) {
            ++args_len;
            ++arg;
        }

        if (args_len > 1) {
            fprintf(stderr, "cash: cd: too many arguments\n");

            free(cmd_name);
            free_args_arr(args._data, args._size);

            return;
        }

        if (args_len == 1) {
            char *arg;
            get_from_dynarr(&arg, &args, 1);

            char *cwd_copy = strdup(cwd);

            // At index 1 due to args list always starting with the cmd's name
            char *dir = malloc(strlen(cwd) + strlen(arg) + 2);

            if (arg[0] == '/') {
                strcpy(dir, arg);
            } else if (arg[0] == '-') {
                if (prev_dir != NULL) {
                    strcpy(dir, prev_dir);
                } else {
                    fprintf(stderr, "cash: cd: prev_dir not set\n");
                }
            } else {
                strcpy(dir, cwd);
                strcat(dir, "/");
                strcat(dir, arg);
            }

            if (chdir(dir) == -1) {
                fprintf(stderr, "cash: cd: %s: %s\n", arg, strerror(errno));
            } else {
                prev_dir = cwd_copy;
            }

            free(dir);
            free(cmd_name);
            free_args_arr(args._data, args._size);

            return;
        }

        char *home_dir = getenv("HOME");
        chdir(home_dir);

        free(cmd_name);
        free_args_arr(args._data, args._size);

        return;
    }

    if (!cmd_abs_path) {
        fprintf(stderr, "cash: %s command not found\n", cmd_name);

        free(cmd_name);
        free_args_arr(args._data, args._size);

        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(
            stderr,
            "cash: unable to execute command: failed to create child process"
            "process\n");
        free(cmd_name);
        free_args_arr(args._data, args._size);

        exit(1);
    } else if (pid == 0) {
        if (execv(cmd_abs_path, args._data)) {
            fprintf(stderr, "cash: %s: %s\n", cmd_name, strerror(errno));
        };
    } else {
        wait(NULL);
    }

    free(cmd_name);
    free_args_arr(args._data, args._size);

    if (cmd_name[0] == '.') {
        free(cmd_abs_path);
    }
}

void free_cmd_abs_paths() {
    for (size_t i = 0; i < cmd_abs_paths._size; ++i) {
        char *cmd_abs_path;
        get_from_dynarr(&cmd_abs_path, &cmd_abs_paths, i);

        free(cmd_abs_path);
    }
}
