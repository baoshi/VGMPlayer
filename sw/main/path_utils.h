#pragma once

#include <stdbool.h>

void path_set_root(char* dir);
bool path_is_root(const char* dir);
bool path_get_parent(const char* path, char* parent);
bool path_get_leaf(const char* qp, char* leaf);
bool path_concatnate(const char* parent, const char* leaf, char* out, bool debracket);