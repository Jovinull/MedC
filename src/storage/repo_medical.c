#include "storage/repo_medical.h"
#include "core/datetime.h"
#include "storage/db.h"
#include <stdio.h>
#include <string.h>

static const char *PATH="data/medical.dat";
static bool ensure_header(void){
  DBHeader h;
  if(db_load_header(PATH,&h)) return true;
  h.magic=0x48534D56u; h.version=1; h.next_id=1; h.count=0;
  return db_write_header(PATH,&h);
}
void repo_medical_init_store(void){ (void)ensure_header(); }

bool repo_medical_find_by_visit(int visit_id, MedicalNote *out){
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  FILE*f=fopen(PATH,"rb"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    MedicalNote m;
    if(fread(&m,sizeof(MedicalNote),1,f)!=1) break;
    if(m.visit_id==visit_id){ *out=m; fclose(f); return true; }
  }
  fclose(f); return false;
}

bool repo_medical_upsert_for_visit(MedicalNote *m){
  ensure_header();
  DBHeader h; if(!db_load_header(PATH,&h)) return false;

  FILE*f=fopen(PATH,"r+b"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    long pos=ftell(f);
    MedicalNote cur;
    if(fread(&cur,sizeof(MedicalNote),1,f)!=1) break;
    if(cur.visit_id==m->visit_id){
      m->id=cur.id;
      m->created_at=cur.created_at;
      fseek(f,pos,SEEK_SET);
      bool ok=(fwrite(m,sizeof(MedicalNote),1,f)==1);
      fclose(f);
      return ok;
    }
  }
  fclose(f);

  m->id=(int)h.next_id++;
  m->created_at=dt_now_unix();
  FILE*fa=fopen(PATH,"ab"); if(!fa) return false;
  if(fwrite(m,sizeof(MedicalNote),1,fa)!=1){ fclose(fa); return false; }
  fclose(fa);
  h.count++;
  return db_write_header(PATH,&h);
}