#include "storage/repo_visits.h"
#include "core/datetime.h"
#include "core/str.h"
#include "storage/db.h"
#include <stdio.h>
#include <string.h>

static const char *PATH="data/visits.dat";
static bool ensure_header(void){
  DBHeader h;
  if(db_load_header(PATH,&h)) return true;
  h.magic=0x48534D56u; h.version=1; h.next_id=1; h.count=0;
  return db_write_header(PATH,&h);
}
void repo_visits_init_store(void){ (void)ensure_header(); }

bool repo_visits_create(Visit *v){
  ensure_header();
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  v->id=(int)h.next_id++;
  v->opened_at=dt_now_unix();
  v->closed_at=0;
  FILE*f=fopen(PATH,"ab"); if(!f) return false;
  if(fwrite(v,sizeof(Visit),1,f)!=1){ fclose(f); return false; }
  fclose(f);
  h.count++;
  return db_write_header(PATH,&h);
}
bool repo_visits_update(const Visit *v){
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  FILE*f=fopen(PATH,"r+b"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    long pos=ftell(f);
    Visit cur;
    if(fread(&cur,sizeof(Visit),1,f)!=1) break;
    if(cur.id==v->id){
      fseek(f,pos,SEEK_SET);
      if(fwrite(v,sizeof(Visit),1,f)!=1){ fclose(f); return false; }
      fclose(f); return true;
    }
  }
  fclose(f); return false;
}
bool repo_visits_find_by_id(int id, Visit *out){
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  FILE*f=fopen(PATH,"rb"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    Visit v;
    if(fread(&v,sizeof(Visit),1,f)!=1) break;
    if(v.id==id){ *out=v; fclose(f); return true; }
  }
  fclose(f); return false;
}
int repo_visits_list(Visit*out,int cap){
  DBHeader h; if(!db_load_header(PATH,&h)) return 0;
  FILE*f=fopen(PATH,"rb"); if(!f) return 0;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  int n=0;
  for(uint32_t i=0;i<h.count && n<cap;i++){
    if(fread(&out[n],sizeof(Visit),1,f)!=1) break;
    n++;
  }
  fclose(f); return n;
}
const char* visit_status_name(VisitStatus s){
  switch(s){
    case VISIT_ABERTO: return "ABERTO";
    case VISIT_EM_TRIAGEM: return "EM TRIAGEM";
    case VISIT_EM_CONSULTA: return "EM CONSULTA";
    case VISIT_ENCERRADO: return "ENCERRADO";
    default: return "N/D";
  }
}
