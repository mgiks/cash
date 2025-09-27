#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynarr.c"

int main(void) {
    DynArr dir_names = new_dynarr(sizeof(char *));

    char *path = getenv("PATH");
    char *token = strtok(path, ":");

    while (token) {
        push_to_dynarr(&dir_names, &token);
        token = strtok(NULL, ":");
    }

    for (size_t i = 0; i < get_size_of_dynarr(&dir_names); ++i) {
        char *dir_name;
        get_from_dynarr(&dir_names, i, &dir_name);

        DIR *d = opendir(dir_name);
        if (d == NULL) {
            printf("Unable to open directory\n");
            exit(1);
        }

        struct dirent *dir;
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
                continue;
            }
            size_t abs_cmd_path_len = strlen(dir_name) + strlen(dir->d_name) +
                                      2;  // Tow extra bytes for '/' and '\0'

            char *abs_cmd_path = malloc(abs_cmd_path_len);

            snprintf(abs_cmd_path, abs_cmd_path_len, "%s/%s", dir_name,
                     dir->d_name);

            printf("%s\n", abs_cmd_path);
            free(abs_cmd_path);
        }

        closedir(d);
    }

    clear_dynarr(&dir_names);

    return 0;
}
