#include "modules/enfermagem/triage_views.h"
#include "storage/repo_visits.h"
#include "storage/repo_patients.h"
#include "core/datetime.h"
#include "ui/widgets.h"
#include "ui/theme.h"
#include <stdio.h>

extern int triage_sel;

void triage_views_draw(App *app, Rect c){
  widgets_draw_table_header(&app->ui,c.y+1,c.x+2,"Enfermagem • Classificação de Risco");
  ui_print(c.y+3,c.x+2,90,49,"[Enter] Abrir triagem do atendimento selecionado");

  Visit list[256]; int n=repo_visits_list(list,256);
  int idxs[256]; int k=0;
  for(int i=0;i<n;i++){
    if(list[i].status==VISIT_EM_TRIAGEM) idxs[k++]=i;
  }
  if(triage_sel>=k) triage_sel = k? k-1:0;

  ui_print(c.y+6,c.x+2,90,49,"ID   Paciente                     Abertura            Queixa");
  int row=c.y+7;
  for(int i=0;i<k && i< (c.h-9); i++){
    Visit *v=&list[idxs[i]];
    Patient p; repo_patients_find_by_id(v->patient_id,&p);
    char dt[20]; dt_format_ymd_hms(dt,v->opened_at);

    int fg=(i==triage_sel)?97:90;
    int bg=(i==triage_sel)?47:49;
    ui_print(row+i,c.x+2,fg,bg,"%4d %-28s %-19s %.32s",
             v->id,p.name,dt,v->complaint);
  }
  if(k==0){
    ui_print(c.y+9,c.x+2,90,49,"Nenhum atendimento aguardando triagem.");
  }
  (void)app;
}