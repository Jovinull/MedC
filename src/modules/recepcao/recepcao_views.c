#include "modules/recepcao/recepcao_views.h"
#include "storage/repo_visits.h"
#include "storage/repo_patients.h"
#include "core/datetime.h"
#include "ui/widgets.h"
#include "ui/theme.h"
#include "core/str.h"
#include <stdio.h>

extern int recepcao_sel;
extern int recepcao_status_filter;
extern char recepcao_search[32];

void recepcao_views_draw(App *app, Rect c){
  widgets_draw_table_header(&app->ui,c.y+1,c.x+2,"Recepção • Atendimentos do dia (MVP)");
  ui_print(c.y+3,c.x+2,90,49,"[N] Novo check-in   [F] Filtro status   [/ ] Buscar queixa   [Enter] Encerrar (se EM CONSULTA)");

  ui_print(c.y+5,c.x+2,90,49,"Filtro status: %s | Busca: %s",
           recepcao_status_filter==0?"TODOS":visit_status_name((VisitStatus)recepcao_status_filter),
           recepcao_search[0]?recepcao_search:"(nenhuma)");

  Visit list[256];
  int n=repo_visits_list(list,256);

  int idxs[256]; int k=0;
  for(int i=0;i<n;i++){
    if(recepcao_status_filter!=0 && (int)list[i].status!=recepcao_status_filter) continue;
    if(recepcao_search[0]!=0 && !str_has(list[i].complaint,recepcao_search)) continue;
    idxs[k++]=i;
  }
  if(recepcao_sel>=k) recepcao_sel = k? k-1:0;

  ui_print(c.y+6,c.x+2,90,49,"ID   Paciente                     Status        Abertura            Queixa");
  int row=c.y+7;
  for(int i=0;i<k && i< (c.h-9); i++){
    Visit *v=&list[idxs[i]];
    Patient p; p.id=0;
    repo_patients_find_by_id(v->patient_id,&p);
    char dt[32]; dt_format_ymd_hms(dt,v->opened_at);

    int fg=(i==recepcao_sel)?97:90;
    int bg=(i==recepcao_sel)?47:49;
    ui_print(row+i,c.x+2,fg,bg,"%4d %-28s %-12s %-19s %.28s",
             v->id,p.name,visit_status_name(v->status),dt,v->complaint);
  }
  if(k==0){
    ui_print(c.y+9,c.x+2,90,49,"Nenhum atendimento. Pressione N para abrir um check-in.");
  }
  (void)app;
}