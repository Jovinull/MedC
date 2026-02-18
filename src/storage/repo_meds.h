#ifndef STORAGE_REPO_MEDS_H
#define STORAGE_REPO_MEDS_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int id;
  char name[64];
  char unit[12];
  int min_stock;
  bool active;
} Med;

void repo_meds_init_store(void);
void repo_meds_seed_defaults(void);

bool repo_meds_create(Med *m);
bool repo_meds_update(const Med *m);
bool repo_meds_find_by_id(int id, Med *out);
int  repo_meds_list(Med *out, int cap);

#endif