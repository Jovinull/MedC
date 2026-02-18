#ifndef STORAGE_REPO_USERS_H
#define STORAGE_REPO_USERS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int id;
  char username[32];
  uint8_t salt[16];
  char password_hash_hex[65];
  int role_id;
  bool active;
  bool must_change_password;
  int64_t created_at;
} User;

void repo_users_init_store(void);

bool repo_users_create(User *u);
bool repo_users_update(const User *u);

bool repo_users_find_by_username(const char *username, User *out);
bool repo_users_find_by_id(int id, User *out);

int repo_users_list(User *out, int cap); /* retorna qtd */

#endif