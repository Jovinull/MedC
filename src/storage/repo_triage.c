#include "storage/repo_triage.h"
#include "core/datetime.h"
#include "core/str.h"
#include "storage/db.h"
#include <stdio.h>
#include <string.h>

static const char *PATH="data/triage.dat";
static bool ensure_header(void){
  DBHeader h;
  if(db_load_header(PATH,&h)) return true;
  h.magic=0x48534D56u; h.version=1; h.next_id=1; h.count=0;
  return db_write_header(PATH,&h);
}
void repo_triage_init_store(void){ (void)ensure_header(); }

bool repo_triage_find_by_visit(int visit_id, Triage *out){
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  FILE*f=fopen(PATH,"rb"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    Triage t;
    if(fread(&t,sizeof(Triage),1,f)!=1) break;
    if(t.visit_id==visit_id){ *out=t; fclose(f); return true; }
  }
  fclose(f); return false;
}

bool repo_triage_upsert_for_visit(Triage *t){
  ensure_header();
  DBHeader h; if(!db_load_header(PATH,&h)) return false;

  /* tenta atualizar */
  FILE*f=fopen(PATH,"r+b");
  if(!f) return false;

  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    long pos=ftell(f);
    Triage cur;
    if(fread(&cur,sizeof(Triage),1,f)!=1) break;
    if(cur.visit_id==t->visit_id){
      t->id=cur.id;
      t->created_at=cur.created_at;
      fseek(f,pos,SEEK_SET);
      bool ok = (fwrite(t,sizeof(Triage),1,f)==1);
      fclose(f);
      return ok;
    }
  }
  fclose(f);

  /* create */
  t->id=(int)h.next_id++;
  t->created_at=dt_now_unix();

  FILE*fa=fopen(PATH,"ab"); if(!fa) return false;
  if(fwrite(t,sizeof(Triage),1,fa)!=1){ fclose(fa); return false; }
  fclose(fa);

  h.count++;
  return db_write_header(PATH,&h);
}