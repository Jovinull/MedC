#ifndef APP_SESSION_H
#define APP_SESSION_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool authenticated;
  int user_id;
  int role_id;
  uint32_t perms;
  char username[32];
  char token[33];
  bool must_change_password;
} Session;

void session_clear(Session *s);
void session_make_token(char out[33]);

#endif