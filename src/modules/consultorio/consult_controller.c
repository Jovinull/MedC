#include "modules/consultorio/consult_controller.h"
#include "modules/consultorio/consult_views.h"
#include "storage/repo_visits.h"
#include "storage/repo_medical.h"
#include "storage/repo_rx.h"
#include "storage/repo_audit.h"
#include "ui/widgets.h"
#include "core/str.h"
#include <string.h>

int consult_sel=0;

void consult_controller_reset(void){
  consult_sel=0;
  repo_visits_init_store();
  repo_medical_init_store();
  repo_rx_init_store();
}

static void consult_fill(App *app, Visit *v){
  InputField f[3];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"Evolução",31);
  strncpy(f[1].label,"Diagnóstico",31);
  strncpy(f[2].label,"Conduta",31);

  MedicalNote cur;
  if(repo_medical_find_by_visit(v->id,&cur)){
    str_safe_copy(f[0].value,sizeof(f[0].value),cur.text);
    str_safe_copy(f[1].value,sizeof(f[1].value),cur.diagnosis);
    str_safe_copy(f[2].value,sizeof(f[2].value),cur.conduct);
  }

  if(!widgets_modal_form(&app->ui,"Consulta • Evolução médica",f,3,"Enter salva")) return;

  MedicalNote m; memset(&m,0,sizeof(m));
  m.visit_id=v->id;
  str_safe_copy(m.text,sizeof(m.text),f[0].value);
  str_safe_copy(m.diagnosis,sizeof(m.diagnosis),f[1].value);
  str_safe_copy(m.conduct,sizeof(m.conduct),f[2].value);

  repo_medical_upsert_for_visit(&m);
  repo_audit_log(app->session.user_id,"CONSULT_WRITE","medical_notes",m.id,"Evolução registrada.");
  ui_toast(&app->ui,"Consulta salva.");
}

static void rx_add_item(App *app, int rx_id){
  InputField f[4];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"Medicamento",31);
  strncpy(f[1].label,"Dose",31);
  strncpy(f[2].label,"Frequência",31);
  strncpy(f[3].label,"Dias",31);

  if(!widgets_modal_form(&app->ui,"Prescrição • Novo item",f,4,"Enter adiciona")) return;

  PrescriptionItem it;
  memset(&it,0,sizeof(it));
  str_safe_copy(it.item_name,sizeof(it.item_name),f[0].value);
  str_safe_copy(it.dose,sizeof(it.dose),f[1].value);
  str_safe_copy(it.frequency,sizeof(it.frequency),f[2].value);
  it.days=atoi(f[3].value);

  repo_rx_items_add(rx_id,&it);
  repo_audit_log(app->session.user_id,"PRESCRIPTION_WRITE","prescription_items",0,"Item de prescrição adicionado.");
  ui_toast(&app->ui,"Item adicionado.");
}

static void rx_manage(App *app, Visit *v){
  Prescription rx;
  repo_rx_get_or_create_for_visit(v->id,&rx);

  while(1){
    PrescriptionItem items[64];
    int n=repo_rx_items_list(rx.id,items,64);

    char msg[200];
    snprintf(msg,sizeof(msg),"Prescrição #%d (itens: %d). Adicionar item?",rx.id,n);

    if(!widgets_modal_confirm(&app->ui,"Prescrição",msg)) break;
    rx_add_item(app,rx.id);
  }
}

static void close_visit(App *app, Visit *v){
  if(v->status != VISIT_ENCERRADO){
    if(!widgets_modal_confirm(&app->ui,"Encerrar","Encerrar atendimento ao finalizar consulta?")) return;
    v->status=VISIT_ENCERRADO;
    v->closed_at=dt_now_unix();
    repo_visits_update(v);
    repo_audit_log(app->session.user_id,"VISIT_CLOSE","visits",v->id,"Encerrado após consulta.");
    ui_toast(&app->ui,"Atendimento encerrado.");
  }
}

void consult_controller_render(App *app, Rect c){ consult_views_draw(app,c); }

void consult_controller_key(App *app, KeyEvent ev){
  Visit list[256]; int n=repo_visits_list(list,256);
  int idxs[256]; int k=0;
  for(int i=0;i<n;i++){
    if(list[i].status==VISIT_EM_CONSULTA) idxs[k++]=i;
  }
  if(consult_sel>=k) consult_sel = k? k-1:0;

  if(ev.type==KEY_UP){ if(consult_sel>0) consult_sel--; return; }
  if(ev.type==KEY_DOWN){ if(consult_sel<k-1) consult_sel++; return; }

  if(ev.type==KEY_CHAR && (ev.ch=='p'||ev.ch=='P') && k>0){
    Visit v=list[idxs[consult_sel]];
    rx_manage(app,&v);
    return;
  }

  if(ev.type==KEY_CHAR && (ev.ch=='e'||ev.ch=='E') && k>0){
    Visit v=list[idxs[consult_sel]];
    consult_fill(app,&v);
    return;
  }

  if(ev.type==KEY_ENTER && k>0){
    Visit v=list[idxs[consult_sel]];
    close_visit(app,&v);
    return;
  }
}