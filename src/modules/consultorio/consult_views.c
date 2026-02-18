#include "modules/consultorio/consult_views.h"
#include "storage/repo_visits.h"
#include "storage/repo_patients.h"
#include "storage/repo_medical.h"
#include "storage/repo_rx.h"
#include "core/datetime.h"
#include "ui/widgets.h"
#include "ui/theme.h"
#include <stdio.h>

extern int sel;

void consult_views_draw(App *app, Rect c){
  widgets_draw_table_header(&app->ui,c.y+1,c.x+2,"Consultório • Atendimentos em consulta");
  ui_print(c.y+3,c.x+2,90,49,"[E] Evolução   [P] Prescrição   [Enter] Encerrar atendimento");

  Visit list[256]; int n=repo_visits_list(list,256);
  int idxs[256]; int k=0;
  for(int i=0;i<n;i++){
    if(list[i].status==VISIT_EM_CONSULTA) idxs[k++]=i;
  }
  if(sel>=k) sel = k? k-1:0;

  ui_print(c.y+6,c.x+2,90,49,"ID   Paciente                     Abertura            Queixa");
  int row=c.y+7;
  for(int i=0;i<k && i< (c.h-12); i++){
    Visit *v=&list[idxs[i]];
    Patient p; repo_patients_find_by_id(v->patient_id,&p);
    char dt[20]; dt_format_ymd_hms(dt,v->opened_at);
    int fg=(i==sel)?97:90;
    int bg=(i==sel)?47:49;
    ui_print(row+i,c.x+2,fg,bg,"%4d %-28s %-19s %.28s",v->id,p.name,dt,v->complaint);
  }

  if(k>0){
    Visit v=list[idxs[sel]];
    MedicalNote m; bool hasM=repo_medical_find_by_visit(v.id,&m);
    Prescription rx; repo_rx_get_or_create_for_visit(v.id,&rx);
    PrescriptionItem items[64]; int ni=repo_rx_items_list(rx.id,items,64);

    ui_print(c.y + c.h - 4, c.x + 2, 90, 49, "Resumo selecionado:");
    ui_print(c.y + c.h - 3, c.x + 2, 90, 49, "Evolução: %s | Prescrição itens: %d", hasM?"SIM":"NÃO", ni);
  } else {
    ui_print(c.y+9,c.x+2,90,49,"Nenhum atendimento em consulta.");
  }
  (void)app;
}