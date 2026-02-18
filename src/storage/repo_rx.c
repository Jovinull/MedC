#include "storage/repo_rx.h"
#include "core/datetime.h"
#include "storage/db.h"
#include <stdio.h>
#include <string.h>

static const char *PATH_RX="data/rx.dat";
static const char *PATH_IT="data/rx_items.dat";

static bool ensure_header(const char *path){
  DBHeader h;
  if(db_load_header(path,&h)) return true;
  h.magic=0x48534D56u; h.version=1; h.next_id=1; h.count=0;
  return db_write_header(path,&h);
}
void repo_rx_init_store(void){ (void)ensure_header(PATH_RX); (void)ensure_header(PATH_IT); }

bool repo_rx_get_or_create_for_visit(int visit_id, Prescription *out){
  ensure_header(PATH_RX);
  DBHeader h; if(!db_load_header(PATH_RX,&h)) return false;

  FILE*f=fopen(PATH_RX,"rb");
  if(f){
    fseek(f,(long)sizeof(DBHeader),SEEK_SET);
    for(uint32_t i=0;i<h.count;i++){
      Prescription p;
      if(fread(&p,sizeof(Prescription),1,f)!=1) break;
      if(p.visit_id==visit_id){ *out=p; fclose(f); return true; }
    }
    fclose(f);
  }

  Prescription p;
  memset(&p,0,sizeof(p));
  p.id=(int)h.next_id++;
  p.visit_id=visit_id;
  p.created_at=dt_now_unix();

  FILE*fa=fopen(PATH_RX,"ab"); if(!fa) return false;
  if(fwrite(&p,sizeof(Prescription),1,fa)!=1){ fclose(fa); return false; }
  fclose(fa);

  h.count++;
  if(!db_write_header(PATH_RX,&h)) return false;
  *out=p;
  return true;
}

int repo_rx_items_list(int prescription_id, PrescriptionItem *out, int cap){
  DBHeader h; if(!db_load_header(PATH_IT,&h)) return 0;
  FILE*f=fopen(PATH_IT,"rb"); if(!f) return 0;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  int n=0;
  for(uint32_t i=0;i<h.count && n<cap;i++){
    PrescriptionItem it;
    if(fread(&it,sizeof(PrescriptionItem),1,f)!=1) break;
    if(it.prescription_id==prescription_id){ out[n++]=it; }
  }
  fclose(f);
  return n;
}

bool repo_rx_items_add(int prescription_id, const PrescriptionItem *it){
  ensure_header(PATH_IT);
  DBHeader h; if(!db_load_header(PATH_IT,&h)) return false;

  PrescriptionItem w = *it;
  w.id=(int)h.next_id++;
  w.prescription_id=prescription_id;

  FILE*f=fopen(PATH_IT,"ab"); if(!f) return false;
  if(fwrite(&w,sizeof(PrescriptionItem),1,f)!=1){ fclose(f); return false; }
  fclose(f);

  h.count++;
  return db_write_header(PATH_IT,&h);
}

bool repo_rx_items_clear(int prescription_id){
  DBHeader h; if(!db_load_header(PATH_IT,&h)) return true;

  FILE*f=fopen(PATH_IT,"rb"); if(!f) return true;
  PrescriptionItem *buf = (PrescriptionItem*)malloc(sizeof(PrescriptionItem) * h.count);
  if(!buf){ fclose(f); return false; }

  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  uint32_t kept=0;
  for(uint32_t i=0;i<h.count;i++){
    PrescriptionItem it;
    if(fread(&it,sizeof(PrescriptionItem),1,f)!=1) break;
    if(it.prescription_id!=prescription_id) buf[kept++]=it;
  }
  fclose(f);

  DBHeader nh=h; nh.count=kept;
  /* não reaproveita ids; mantém next_id */
  FILE*fw=fopen(PATH_IT,"wb"); if(!fw){ free(buf); return false; }
  fwrite(&nh,sizeof(DBHeader),1,fw);
  if(kept>0) fwrite(buf,sizeof(PrescriptionItem),kept,fw);
  fclose(fw);
  free(buf);
  return true;
}