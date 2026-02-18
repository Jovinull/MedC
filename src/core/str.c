#include "core/str.h"

#include <ctype.h>
#include <string.h>

bool str_eq(const char *a, const char *b) {
  if (!a || !b) return false;
  return strcmp(a, b) == 0;
}

static int lowerc(int c) { return tolower((unsigned char)c); }

bool str_has(const char *hay, const char *needle) {
  if (!hay || !needle) return false;
  size_t nh = strlen(hay), nn = strlen(needle);
  if (nn == 0) return true;
  for (size_t i = 0; i + nn <= nh; i++) {
    size_t j = 0;
    for (; j < nn; j++) {
      if (lowerc(hay[i + j]) != lowerc(needle[j])) break;
    }
    if (j == nn) return true;
  }
  return false;
}

void str_trim(char *s) {
  if (!s) return;
  size_t n = strlen(s);
  while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ' || s[n - 1] == '\t')) {
    s[n - 1] = '\0';
    n--;
  }
  size_t i = 0;
  while (s[i] == ' ' || s[i] == '\t') i++;
  if (i > 0) memmove(s, s + i, strlen(s + i) + 1);
}

void str_safe_copy(char *dst, size_t dstsz, const char *src) {
  if (!dst || dstsz == 0) return;
  if (!src) {
    dst[0] = '\0';
    return;
  }
  size_t n = strlen(src);
  if (n >= dstsz) n = dstsz - 1;
  memcpy(dst, src, n);
  dst[n] = '\0';
}