#ifndef STORAGE_REPO_VISITS_H
#define STORAGE_REPO_VISITS_H
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  VISIT_ABERTO=1,
  VISIT_EM_TRIAGEM=2,
  VISIT_EM_CONSULTA=3,
  VISIT_ENCERRADO=4
} VisitStatus;

typedef struct {
  int id;
  int patient_id;
  int64_t opened_at;
  int64_t closed_at;
  VisitStatus status;
  char complaint[120];
} Visit;

void repo_visits_init_store(void);
bool repo_visits_create(Visit *v);
bool repo_visits_update(const Visit *v);
bool repo_visits_find_by_id(int id, Visit *out);
int  repo_visits_list(Visit *out, int cap);

const char* visit_status_name(VisitStatus s);

#endif