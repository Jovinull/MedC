#include "storage/repo_patients.h"
#include "core/datetime.h"
#include "core/str.h"
#include "storage/db.h"
#include <stdio.h>
#include <string.h>

static const char *PATH = "data/patients.dat";
static bool ensure_header(void){
  DBHeader h;
  if (db_load_header(PATH,&h)) return true;
  h.magic=0x48534D56u; h.version=1; h.next_id=1; h.count=0;
  return db_write_header(PATH,&h);
}
void repo_patients_init_store(void){ (void)ensure_header(); }

bool repo_patients_create(Patient *p){
  ensure_header();
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  p->id=(int)h.next_id++; p->created_at=dt_now_unix(); p->active=true;

  FILE*f=fopen(PATH,"ab"); if(!f) return false;
  if(fwrite(p,sizeof(Patient),1,f)!=1){ fclose(f); return false; }
  fclose(f);

  h.count++;
  return db_write_header(PATH,&h);
}
bool repo_patients_update(const Patient *p){
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  FILE*f=fopen(PATH,"r+b"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    long pos=ftell(f);
    Patient cur;
    if(fread(&cur,sizeof(Patient),1,f)!=1) break;
    if(cur.id==p->id){
      fseek(f,pos,SEEK_SET);
      if(fwrite(p,sizeof(Patient),1,f)!=1){ fclose(f); return false; }
      fclose(f); return true;
    }
  }
  fclose(f); return false;
}
bool repo_patients_find_by_id(int id, Patient *out){
  DBHeader h; if(!db_load_header(PATH,&h)) return false;
  FILE*f=fopen(PATH,"rb"); if(!f) return false;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  for(uint32_t i=0;i<h.count;i++){
    Patient p;
    if(fread(&p,sizeof(Patient),1,f)!=1) break;
    if(p.id==id){ *out=p; fclose(f); return true; }
  }
  fclose(f); return false;
}
int repo_patients_list(Patient*out,int cap){
  DBHeader h; if(!db_load_header(PATH,&h)) return 0;
  FILE*f=fopen(PATH,"rb"); if(!f) return 0;
  fseek(f,(long)sizeof(DBHeader),SEEK_SET);
  int n=0;
  for(uint32_t i=0;i<h.count && n<cap;i++){
    if(fread(&out[n],sizeof(Patient),1,f)!=1) break;
    n++;
  }
  fclose(f);
  return n;
}