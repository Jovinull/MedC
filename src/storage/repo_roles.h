#ifndef STORAGE_REPO_ROLES_H
#define STORAGE_REPO_ROLES_H

#include <stdbool.h>

typedef struct {
  int id;
  char name[24];
} Role;

void repo_roles_init_store(void);
void repo_roles_seed_defaults(void);

bool repo_roles_find_by_id(int id, Role *out);

#endif