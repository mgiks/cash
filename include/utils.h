#ifndef HELPER_H
#define HELPER_H

void strrev(char *s);

void trim_leading_space(char **str);

char *trim_path(char *path);

char *rel_to_abs_path(char *rel_path, char *cwd);

char *get_home_dir();

#endif
