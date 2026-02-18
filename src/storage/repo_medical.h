#ifndef STORAGE_REPO_MEDICAL_H
#define STORAGE_REPO_MEDICAL_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int id;
  int visit_id;
  char text[260];
  char diagnosis[160];
  char conduct[200];
  int64_t created_at;
} MedicalNote;

void repo_medical_init_store(void);
bool repo_medical_upsert_for_visit(MedicalNote *m);
bool repo_medical_find_by_visit(int visit_id, MedicalNote *out);

#endif
