#ifndef CORE_COMMON_H
#define CORE_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

typedef struct {
  int x, y, w, h;
} Rect;

typedef enum {
  STATUS_OK = 0,
  STATUS_ERR = 1
} Status;

#endif