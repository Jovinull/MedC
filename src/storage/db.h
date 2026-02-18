#ifndef STORAGE_DB_H
#define STORAGE_DB_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t magic;
  uint32_t version;
  uint32_t next_id;
  uint32_t count;
} DBHeader;

bool db_load_header(const char *path, DBHeader *out);
bool db_write_header(const char *path, const DBHeader *h);

#endif