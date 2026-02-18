#ifndef STORAGE_REPO_STOCK_H
#define STORAGE_REPO_STOCK_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  int id;
  int meds_id;
  char lot[24];
  char expiry[11]; /* YYYY-MM-DD ou vazio */
  int qty;
  float unit_cost;
  int64_t created_at;
} StockLot;

typedef enum { MOVE_IN=1, MOVE_OUT=2 } MoveType;

typedef struct {
  int id;
  int meds_id;
  int lot_id;      /* 0 se manual sem lote */
  MoveType type;
  int qty;
  char reason[80];
  int visit_id;    /* 0 se não vinculado */
  int64_t created_at;
} StockMove;

void repo_stock_init_store(void);
bool repo_stock_lot_create(StockLot *l);
bool repo_stock_lot_update(const StockLot *l);
bool repo_stock_lot_find_by_id(int id, StockLot *out);
int  repo_stock_lots_list_by_meds(int meds_id, StockLot *out, int cap);

bool repo_stock_move_create(StockMove *m);
int  repo_stock_moves_list(StockMove *out, int cap);

int repo_stock_qty_total_for_meds(int meds_id);
int repo_stock_qty_lot(int lot_id);

#endif
