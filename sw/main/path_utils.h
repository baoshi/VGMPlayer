#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void path_set_root_dir(char *path);
void path_set_null(char *path);
bool path_is_root_dir(const char *path);
bool path_is_null(const char *path);
bool path_get_parent(const char *path, char *parent, int len);
bool path_get_leaf(const char *path, char *leaf, int len);

bool path_concatenate(const char *parent, const char *leaf, char *out, int len, bool debracket);
bool path_get_ext(const char *file, char *ext, int len);
bool path_change_ext(const char *file, const char *ext, char *out, int len);
void path_duplicate(const char *src, char *dest, int len);
bool path_trim_back(char *path);

#ifdef __cplusplus
}
#endif