#ifndef CORE_STR_H
#define CORE_STR_H

#include <stdbool.h>
#include <stddef.h>

bool str_eq(const char *a, const char *b);
bool str_has(const char *hay, const char *needle); /* case-insensitive simples */
void str_trim(char *s);
void str_safe_copy(char *dst, size_t dstsz, const char *src);

#endif