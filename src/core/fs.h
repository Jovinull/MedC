#ifndef CORE_FS_H
#define CORE_FS_H

#include <stdbool.h>

bool fs_exists(const char *path);
bool fs_mkdir_if_missing(const char *path);
bool fs_write_all(const char *path, const void *data, unsigned long size);
bool fs_read_all(const char *path, void **out_data, unsigned long *out_size);

#endif