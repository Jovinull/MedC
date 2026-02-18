#ifndef STORAGE_REPO_AUDIT_H
#define STORAGE_REPO_AUDIT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int id;
  int user_id;
  char action[24];
  char entity[24];
  int entity_id;
  int64_t ts;
  char details[120];
} Audit;

bool repo_audit_log(int user_id, const char *action, const char *entity, int entity_id, const char *details);
int repo_audit_list_latest(Audit *out, int cap);

#endif