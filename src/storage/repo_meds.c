#include "storage/repo_meds.h"
#include "core/str.h"
#include "storage/db.h"
#include <stdio.h>
#include <string.h>

static const char *PATH="data/meds.dat";
static bool ensure_header(void){
  DBHeader h;
  if(db_load_header(PATH,&h)) return true;
  h.magic=0x48534D56u; h.version=1; h.next_id=1; h.count=0;
  return db_write_header(PATH,&h);
}
void repo_meds_init_store(void){ (void)ensure_header(); }

bool repo_meds_create(Med *m){
  ensure_header();
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  m->id=(int)h.next_id++;
  if(!m->active) m->active=true;
  FILE*f=fopen(PATH,"ab"); if(!f) return false;
  if(fwrite(m,sizeof(Med),1,f)!=1){ fclose(f); return false; }
  fclose(f);
  h.count++;
  return db_write_header(PATH,&h);
}
bool repo_meds_update(const Med *m){
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  FILE*f=fopen(PATH,"r+b"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    long pos=ftell(f);
    Med cur;
    if(fread(&cur,sizeof(Med),1,f)!=1) break;
    if(cur.id==m->id){
      fseek(f,pos,SEEK_SET);
      bool ok=(fwrite(m,sizeof(Med),1,f)==1);
      fclose(f);
      return ok;
    }
  }
  fclose(f); return false;
}
bool repo_meds_find_by_id(int id, Med *out){
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  FILE*f=fopen(PATH,"rb"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    Med m;
    if(fread(&m,sizeof(Med),1,f)!=1) break;
    if(m.id==id){ *out=m; fclose(f); return true; }
  }
  fclose(f); return false;
}
int repo_meds_list(Med*out,int cap){
  DBHeader h; if(!db_load_header(PATH,&h)) return 0;
  FILE*f=fopen(PATH,"rb"); if(!f) return 0;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  int n=0;
  for(uint32_t i=0;i<h.count && n<cap;i++){
    if(fread(&out[n],sizeof(Med),1,f)!=1) break;
    n++;
  }
  fclose(f); return n;
}
void repo_meds_seed_defaults(void){
  repo_meds_init_store();
  Med m;
  memset(&m,0,sizeof(m));
  str_safe_copy(m.name,sizeof(m.name),"DIPIRONA 500MG"); str_safe_copy(m.unit,sizeof(m.unit),"amp"); m.min_stock=10; m.active=true; repo_meds_create(&m);
  memset(&m,0,sizeof(m));
  str_safe_copy(m.name,sizeof(m.name),"TRAMADOL 50MG/ML"); str_safe_copy(m.unit,sizeof(m.unit),"amp"); m.min_stock=5; m.active=true; repo_meds_create(&m);
  memset(&m,0,sizeof(m));
  str_safe_copy(m.name,sizeof(m.name),"SORO FISIOLOGICO 0,9%"); str_safe_copy(m.unit,sizeof(m.unit),"bol"); m.min_stock=20; m.active=true; repo_meds_create(&m);
}
