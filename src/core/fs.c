#include "core/fs.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(p) mkdir((p), 0755)
#endif

bool fs_exists(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return false;
  fclose(f);
  return true;
}

bool fs_mkdir_if_missing(const char *path) {
  FILE *f = fopen(path, "rb");
  if (f) {
    fclose(f);
    return true;
  }
  if (errno == ENOENT) {
    if (MKDIR(path) == 0) return true;
    /* se já existe, ok */
    return true;
  }
  return true;
}

bool fs_write_all(const char *path, const void *data, unsigned long size) {
  FILE *f = fopen(path, "wb");
  if (!f) return false;
  if (size > 0 && fwrite(data, 1, size, f) != size) {
    fclose(f);
    return false;
  }
  fclose(f);
  return true;
}

bool fs_read_all(const char *path, void **out_data, unsigned long *out_size) {
  *out_data = NULL;
  *out_size = 0;

  FILE *f = fopen(path, "rb");
  if (!f) return false;
  if (fseek(f, 0, SEEK_END) != 0) {
    fclose(f);
    return false;
  }
  long sz = ftell(f);
  if (sz < 0) {
    fclose(f);
    return false;
  }
  if (fseek(f, 0, SEEK_SET) != 0) {
    fclose(f);
    return false;
  }

  void *buf = NULL;
  if (sz > 0) {
    buf = malloc((size_t)sz);
    if (!buf) {
      fclose(f);
      return false;
    }
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) {
      free(buf);
      fclose(f);
      return false;
    }
  }
  fclose(f);

  *out_data = buf;
  *out_size = (unsigned long)sz;
  return true;
}