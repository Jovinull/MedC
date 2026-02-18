#include "app/session.h"
#include <string.h>
#include <time.h>

void session_clear(Session *s) {
  memset(s, 0, sizeof(*s));
}

void session_make_token(char out[33]) {
  static const char *hex = "0123456789abcdef";
  uint64_t t = (uint64_t)time(NULL);
  for (int i = 0; i < 32; i++) {
    unsigned v = (unsigned)((t >> ((i % 8) * 8)) ^ (uint64_t)(0xB7 + i * 29));
    out[i] = hex[v & 0xF];
  }
  out[32] = '\0';
}