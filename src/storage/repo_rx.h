#ifndef STORAGE_REPO_RX_H
#define STORAGE_REPO_RX_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int id;
  int visit_id;
  int64_t created_at;
} Prescription;

typedef struct {
  int id;
  int prescription_id;
  char item_name[64];
  char dose[24];
  char frequency[24];
  int days;
} PrescriptionItem;

void repo_rx_init_store(void);
bool repo_rx_get_or_create_for_visit(int visit_id, Prescription *out);
int  repo_rx_items_list(int prescription_id, PrescriptionItem *out, int cap);
bool repo_rx_items_add(int prescription_id, const PrescriptionItem *it);
bool repo_rx_items_clear(int prescription_id);

#endif