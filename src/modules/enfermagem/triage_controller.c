#include "modules/enfermagem/triage_controller.h"
#include "modules/enfermagem/triage_views.h"
#include "storage/repo_visits.h"
#include "storage/repo_triage.h"
#include "storage/repo_audit.h"
#include "ui/widgets.h"
#include "core/str.h"
#include <string.h>

int triage_sel=0;

void triage_controller_reset(void){
  triage_sel=0;
  repo_visits_init_store();
  repo_triage_init_store();
}

static void triage_fill(App *app, Visit *v){
  InputField f[9];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"PA Sis",31);
  strncpy(f[1].label,"PA Dia",31);
  strncpy(f[2].label,"FC",31);
  strncpy(f[3].label,"FR",31);
  strncpy(f[4].label,"Temp",31);
  strncpy(f[5].label,"SpO2",31);
  strncpy(f[6].label,"Risco",31);
  strncpy(f[7].label,"Prioridade(1-4)",31);
  strncpy(f[8].label,"Obs",31);

  if(!widgets_modal_form(&app->ui,"Triagem",f,9,"Risco: Azul/Verde/Amarelo/Vermelho")) return;

  Triage t;
  memset(&t,0,sizeof(t));
  t.visit_id=v->id;
  t.pa_sis=atoi(f[0].value);
  t.pa_dia=atoi(f[1].value);
  t.fc=atoi(f[2].value);
  t.fr=atoi(f[3].value);
  t.temp=(float)atof(f[4].value);
  t.spo2=atoi(f[5].value);
  str_safe_copy(t.risk,sizeof(t.risk),f[6].value);
  t.priority=atoi(f[7].value);
  str_safe_copy(t.notes,sizeof(t.notes),f[8].value);

  repo_triage_upsert_for_visit(&t);

  v->status = VISIT_EM_CONSULTA;
  repo_visits_update(v);

  repo_audit_log(app->session.user_id,"TRIAGE_WRITE","triage",t.id,"Triagem registrada.");
  ui_toast(&app->ui,"Triagem salva. Atendimento enviado para consulta.");
}

void triage_controller_render(App *app, Rect c){ triage_views_draw(app,c); }

void triage_controller_key(App *app, KeyEvent ev){
  Visit list[256]; int n=repo_visits_list(list,256);
  int idxs[256]; int k=0;
  for(int i=0;i<n;i++){
    if(list[i].status==VISIT_EM_TRIAGEM) idxs[k++]=i;
  }
  if(triage_sel>=k) triage_sel = k? k-1:0;

  if(ev.type==KEY_UP){ if(triage_sel>0) triage_sel--; return; }
  if(ev.type==KEY_DOWN){ if(triage_sel<k-1) triage_sel++; return; }
  if(ev.type==KEY_ENTER && k>0){
    Visit v=list[idxs[triage_sel]];
    triage_fill(app,&v);
    return;
  }
}