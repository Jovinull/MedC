#ifndef STORAGE_REPO_TRIAGE_H
#define STORAGE_REPO_TRIAGE_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int id;
  int visit_id;
  char risk[12]; /* Azul/Verde/Amarelo/Vermelho */
  int priority;  /* 1..4 */
  int pa_sis;
  int pa_dia;
  int fc;
  int fr;
  float temp;
  int spo2;
  char notes[160];
  int64_t created_at;
} Triage;

void repo_triage_init_store(void);
bool repo_triage_upsert_for_visit(Triage *t);
bool repo_triage_find_by_visit(int visit_id, Triage *out);

#endif