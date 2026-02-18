
#include "storage/repo_stock.h"
#include "core/datetime.h"
#include "storage/db.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *PATH_LOTS="data/stock_lots.dat";
static const char *PATH_MOVES="data/stock_moves.dat";

static bool ensure_header(const char *path){
  DBHeader h;
  if(db_load_header(path,&h)) return true;
  h.magic=0x48534D56u; h.version=1; h.next_id=1; h.count=0;
  return db_write_header(path,&h);
}

void repo_stock_init_store(void){ (void)ensure_header(PATH_LOTS); (void)ensure_header(PATH_MOVES); }

bool repo_stock_lot_create(StockLot *l){
  ensure_header(PATH_LOTS);
  DBHeader h; if(!db_load_header(PATH_LOTS,&h)) return false;
  l->id=(int)h.next_id++;
  l->created_at=dt_now_unix();
  FILE*f=fopen(PATH_LOTS,"ab"); if(!f) return false;
  if(fwrite(l,sizeof(StockLot),1,f)!=1){ fclose(f); return false; }
  fclose(f);
  h.count++;
  return db_write_header(PATH_LOTS,&h);
}

bool repo_stock_lot_update(const StockLot *l){
  DBHeader h; if(!db_load_header(PATH_LOTS,&h)) return false;
  FILE*f=fopen(PATH_LOTS,"r+b"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    long pos=ftell(f);
    StockLot cur;
    if(fread(&cur,sizeof(StockLot),1,f)!=1) break;
    if(cur.id==l->id){
      fseek(f,pos,SEEK_SET);
      bool ok=(fwrite(l,sizeof(StockLot),1,f)==1);
      fclose(f);
      return ok;
    }
  }
  fclose(f);
  return false;
}

bool repo_stock_lot_find_by_id(int id, StockLot *out){
  DBHeader h; if(!db_load_header(PATH_LOTS,&h)) return false;
  FILE*f=fopen(PATH_LOTS,"rb"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    StockLot l;
    if(fread(&l,sizeof(StockLot),1,f)!=1) break;
    if(l.id==id){ *out=l; fclose(f); return true; }
  }
  fclose(f); return false;
}

int repo_stock_lots_list_by_meds(int meds_id, StockLot*out,int cap){
  DBHeader h; if(!db_load_header(PATH_LOTS,&h)) return 0;
  FILE*f=fopen(PATH_LOTS,"rb"); if(!f) return 0;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  int n=0;
  for(uint32_t i=0;i<h.count && n<cap;i++){
    StockLot l;
    if(fread(&l,sizeof(StockLot),1,f)!=1) break;
    if(l.meds_id==meds_id) out[n++]=l;
  }
  fclose(f);
  return n;
}

bool repo_stock_move_create(StockMove *m){
  ensure_header(PATH_MOVES);
  DBHeader h; if(!db_load_header(PATH_MOVES,&h)) return false;

  m->id=(int)h.next_id++;
  m->created_at=dt_now_unix();

  FILE*f=fopen(PATH_MOVES,"ab"); if(!f) return false;
  if(fwrite(m,sizeof(StockMove),1,f)!=1){ fclose(f); return false; }
  fclose(f);

  h.count++;
  if(!db_write_header(PATH_MOVES,&h)) return false;

  /* aplica efeito no lote se existir */
  if(m->lot_id > 0){
    StockLot lot;
    if(repo_stock_lot_find_by_id(m->lot_id,&lot)){
      if(m->type==MOVE_IN) lot.qty += m->qty;
      else lot.qty -= m->qty;
      if(lot.qty < 0) lot.qty = 0;
      repo_stock_lot_update(&lot);
    }
  }
  return true;
}

int repo_stock_moves_list(StockMove*out,int cap){
  DBHeader h; if(!db_load_header(PATH_MOVES,&h)) return 0;
  FILE*f=fopen(PATH_MOVES,"rb"); if(!f) return 0;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  int n=0;
  for(uint32_t i=0;i<h.count && n<cap;i++){
    if(fread(&out[n],sizeof(StockMove),1,f)!=1) break;
    n++;
  }
  fclose(f);
  return n;
}

int repo_stock_qty_lot(int lot_id){
  StockLot l;
  if(!repo_stock_lot_find_by_id(lot_id,&l)) return 0;
  return l.qty;
}

int repo_stock_qty_total_for_meds(int meds_id){
  DBHeader h; if(!db_load_header(PATH_LOTS,&h)) return 0;
  FILE*f=fopen(PATH_LOTS,"rb"); if(!f) return 0;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  int total=0;
  for(uint32_t i=0;i<h.count;i++){
    StockLot l;
    if(fread(&l,sizeof(StockLot),1,f)!=1) break;
    if(l.meds_id==meds_id) total += l.qty;
  }
  fclose(f);
  return total;
}
