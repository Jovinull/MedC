#include "modules/recepcao/recepcao_controller.h"
#include "modules/recepcao/recepcao_views.h"
#include "storage/repo_patients.h"
#include "storage/repo_visits.h"
#include "storage/repo_audit.h"
#include "core/datetime.h"
#include "core/str.h"
#include "ui/widgets.h"
#include <stdlib.h>
#include <string.h>

int recepcao_sel=0;
int recepcao_status_filter=0; /* 0=todos, 1..4 */
char recepcao_search[32];

void recepcao_controller_reset(void){
  recepcao_sel=0; recepcao_status_filter=0; recepcao_search[0]=0;
  repo_patients_init_store(); repo_visits_init_store();
}

static void create_checkin(App *app){
  InputField f[2];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"ID Paciente",31);
  strncpy(f[1].label,"Queixa",31);

  if(!widgets_modal_form(&app->ui,"Check-in • Abrir Atendimento",f,2,"Informe paciente e queixa")) return;

  int pid = atoi(f[0].value);
  Patient p;
  if(!repo_patients_find_by_id(pid,&p) || !p.active){
    ui_toast(&app->ui,"Paciente inválido.");
    return;
  }

  Visit v;
  memset(&v,0,sizeof(v));
  v.patient_id = pid;
  v.status = VISIT_EM_TRIAGEM;
  str_safe_copy(v.complaint,sizeof(v.complaint),f[1].value);

  repo_visits_create(&v);
  repo_audit_log(app->session.user_id,"CHECKIN_CREATE","visits",v.id,"Atendimento aberto.");
  ui_toast(&app->ui,"Atendimento aberto e enviado para triagem.");
}

static void close_visit(App *app, Visit *v){
  if(v->status != VISIT_ENCERRADO){
    if(!widgets_modal_confirm(&app->ui,"Encerrar","Confirma encerrar atendimento?")) return;
    v->status = VISIT_ENCERRADO;
    v->closed_at = dt_now_unix();
    repo_visits_update(v);
    repo_audit_log(app->session.user_id,"VISIT_CLOSE","visits",v->id,"Atendimento encerrado.");
    ui_toast(&app->ui,"Atendimento encerrado.");
  }
}

void recepcao_controller_render(App *app, Rect c){ recepcao_views_draw(app,c); }

void recepcao_controller_key(App *app, KeyEvent ev){
  Visit list[256];
  int n = repo_visits_list(list,256);

  int idxs[256]; int k=0;
  for(int i=0;i<n;i++){
    if(recepcao_status_filter!=0 && (int)list[i].status!=recepcao_status_filter) continue;
    if(recepcao_search[0]!=0 && !str_has(list[i].complaint,recepcao_search)) continue;
    idxs[k++]=i;
  }
  if(recepcao_sel>=k) recepcao_sel = k? k-1:0;

  if(ev.type==KEY_UP){ if(recepcao_sel>0) recepcao_sel--; return; }
  if(ev.type==KEY_DOWN){ if(recepcao_sel<k-1) recepcao_sel++; return; }

  if(ev.type==KEY_CHAR && (ev.ch=='n'||ev.ch=='N')){ create_checkin(app); return; }

  if(ev.type==KEY_CHAR && (ev.ch=='f'||ev.ch=='F')){
    /* alterna filtros de status */
    recepcao_status_filter = (recepcao_status_filter + 1) % 5;
    recepcao_sel=0;
    return;
  }

  if(ev.type==KEY_SLASH){
    InputField f[1]; memset(f,0,sizeof(f));
    strncpy(f[0].label,"Buscar queixa",31);
    str_safe_copy(f[0].value,sizeof(f[0].value),recepcao_search);
    if(widgets_modal_form(&app->ui,"Busca",f,1,"Enter aplica")){
      str_safe_copy(recepcao_search,sizeof(recepcao_search),f[0].value);
      recepcao_sel=0;
    }
    return;
  }

  if(ev.type==KEY_ENTER && k>0){
    Visit v = list[idxs[recepcao_sel]];
    if(v.status == VISIT_ENCERRADO){
      ui_toast(&app->ui,"Já encerrado.");
      return;
    }
    /* recepção pode encerrar, mas validação simples: só se passou por consulta */
    if(v.status != VISIT_ENCERRADO && v.status != VISIT_EM_CONSULTA){
      ui_toast(&app->ui,"Só encerra após consulta (status EM CONSULTA).");
      return;
    }
    close_visit(app,&v);
    return;
  }
}