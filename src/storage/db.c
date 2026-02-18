#include "storage/db.h"
#include "core/fs.h"
#include <stdio.h>
#include <string.h>

#define DB_MAGIC 0x48534D56u /* 'HSMV' */

bool db_load_header(const char *path, DBHeader *out) {
  memset(out, 0, sizeof(*out));
  FILE *f = fopen(path, "rb");
  if (!f) return false;
  if (fread(out, sizeof(DBHeader), 1, f) != 1) {
    fclose(f);
    return false;
  }
  fclose(f);
  if (out->magic != DB_MAGIC) return false;
  if (out->version != 1u) return false;
  return true;
}

bool db_write_header(const char *path, const DBHeader *h) {
  FILE *f = fopen(path, "r+b");
  if (!f) {
    f = fopen(path, "wb");
    if (!f) return false;
  }
  if (fwrite(h, sizeof(DBHeader), 1, f) != 1) {
    fclose(f);
    return false;
  }
  fclose(f);
  return true;
}