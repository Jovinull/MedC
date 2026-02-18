#include "modules/cadastros/patients_controller.h"
#include "modules/cadastros/patients_views.h"
#include "storage/repo_patients.h"
#include "storage/repo_audit.h"
#include "core/str.h"
#include "ui/widgets.h"
#include "ui/theme.h"
#include <string.h>
#include <stdio.h>

int patients_sel=0;
char patients_filter[32];

void patients_controller_reset(void){
  patients_sel=0;
  patients_filter[0]='\0';
  repo_patients_init_store();
}

void patients_controller_render(App *app, Rect c){
  patients_views_draw(app,c);
}

static void create_patient(App *app){
  InputField f[7];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"Nome",31);
  strncpy(f[1].label,"CPF",31);
  strncpy(f[2].label,"CNS",31);
  strncpy(f[3].label,"Nasc(YYYY-MM-DD)",31);
  strncpy(f[4].label,"Sexo",31);
  strncpy(f[5].label,"Telefone",31);
  strncpy(f[6].label,"Endereço",31);

  if(!widgets_modal_form(&app->ui,"Novo Paciente",f,7,"Preencha e Enter para salvar")) return;

  Patient p; memset(&p,0,sizeof(p));
  str_safe_copy(p.name,sizeof(p.name),f[0].value);
  str_safe_copy(p.cpf,sizeof(p.cpf),f[1].value);
  str_safe_copy(p.cns,sizeof(p.cns),f[2].value);
  str_safe_copy(p.birth_date,sizeof(p.birth_date),f[3].value);
  str_safe_copy(p.sex,sizeof(p.sex),f[4].value);
  str_safe_copy(p.phone,sizeof(p.phone),f[5].value);
  str_safe_copy(p.address,sizeof(p.address),f[6].value);

  if(strlen(p.name)<2){
    ui_toast(&app->ui,"Nome inválido.");
    return;
  }
  repo_patients_create(&p);
  repo_audit_log(app->session.user_id,"PATIENT_CREATE","patients",p.id,"Paciente criado.");
  ui_toast(&app->ui,"Paciente cadastrado.");
}

static void edit_patient(App *app, Patient *p){
  InputField f[7];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"Nome",31); str_safe_copy(f[0].value,sizeof(f[0].value),p->name);
  strncpy(f[1].label,"CPF",31); str_safe_copy(f[1].value,sizeof(f[1].value),p->cpf);
  strncpy(f[2].label,"CNS",31); str_safe_copy(f[2].value,sizeof(f[2].value),p->cns);
  strncpy(f[3].label,"Nasc",31); str_safe_copy(f[3].value,sizeof(f[3].value),p->birth_date);
  strncpy(f[4].label,"Sexo",31); str_safe_copy(f[4].value,sizeof(f[4].value),p->sex);
  strncpy(f[5].label,"Telefone",31); str_safe_copy(f[5].value,sizeof(f[5].value),p->phone);
  strncpy(f[6].label,"Endereço",31); str_safe_copy(f[6].value,sizeof(f[6].value),p->address);

  if(!widgets_modal_form(&app->ui,"Editar Paciente",f,7,"Enter salva | Esc cancela")) return;

  str_safe_copy(p->name,sizeof(p->name),f[0].value);
  str_safe_copy(p->cpf,sizeof(p->cpf),f[1].value);
  str_safe_copy(p->cns,sizeof(p->cns),f[2].value);
  str_safe_copy(p->birth_date,sizeof(p->birth_date),f[3].value);
  str_safe_copy(p->sex,sizeof(p->sex),f[4].value);
  str_safe_copy(p->phone,sizeof(p->phone),f[5].value);
  str_safe_copy(p->address,sizeof(p->address),f[6].value);

  repo_patients_update(p);
  repo_audit_log(app->session.user_id,"PATIENT_UPDATE","patients",p->id,"Paciente atualizado.");
  ui_toast(&app->ui,"Paciente atualizado.");
}

void patients_controller_key(App *app, KeyEvent ev){
  Patient list[256];
  int n = repo_patients_list(list,256);

  /* filtra */
  int idxs[256]; int k=0;
  for(int i=0;i<n;i++){
    if(!list[i].active) continue;
    if(patients_filter[0]==0 || str_has(list[i].name,patients_filter) || str_has(list[i].cpf,patients_filter)) idxs[k++]=i;
  }
  if(patients_sel>=k) patients_sel = k>0 ? k-1 : 0;

  if(ev.type==KEY_UP){ if(patients_sel>0) patients_sel--; return; }
  if(ev.type==KEY_DOWN){ if(patients_sel<k-1) patients_sel++; return; }

  if(ev.type==KEY_SLASH){
    InputField f[1]; memset(f,0,sizeof(f));
    strncpy(f[0].label,"Buscar",31);
    str_safe_copy(f[0].value,sizeof(f[0].value),patients_filter);
    if(widgets_modal_form(&app->ui,"Busca",f,1,"Digite e Enter")) {
      str_safe_copy(patients_filter,sizeof(patients_filter),f[0].value);
      patients_sel=0;
    }
    return;
  }

  if(ev.type==KEY_CHAR && (ev.ch=='n' || ev.ch=='N')){
    create_patient(app);
    return;
  }

  if(ev.type==KEY_ENTER && k>0){
    Patient p = list[idxs[patients_sel]];
    edit_patient(app,&p);
    return;
  }
}