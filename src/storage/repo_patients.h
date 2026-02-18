#ifndef STORAGE_REPO_PATIENTS_H
#define STORAGE_REPO_PATIENTS_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int id;
  char name[64];
  char cpf[16];
  char cns[20];
  char birth_date[11]; /* YYYY-MM-DD */
  char sex[8];         /* M/F/Outro */
  char phone[20];
  char address[80];
  int64_t created_at;
  bool active;
} Patient;

void repo_patients_init_store(void);
bool repo_patients_create(Patient *p);
bool repo_patients_update(const Patient *p);
bool repo_patients_find_by_id(int id, Patient *out);
int  repo_patients_list(Patient *out, int cap);

#endif