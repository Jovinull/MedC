#include "modules/indicadores/indicators_views.h"
#include "storage/repo_visits.h"
#include "core/datetime.h"
#include "ui/widgets.h"
#include "ui/theme.h"
#include <stdio.h>

void indicators_views_draw(App *app, Rect c){
  widgets_draw_table_header(&app->ui,c.y+1,c.x+2,"Indicadores • Painel do dia (MVP)");

  Visit v[256]; int n=repo_visits_list(v,256);
  int aberto=0, tri=0, cons=0, enc=0;
  long total_time=0; int closed_count=0;

  for(int i=0;i<n;i++){
    if(v[i].status==VISIT_ABERTO) aberto++;
    else if(v[i].status==VISIT_EM_TRIAGEM) tri++;
    else if(v[i].status==VISIT_EM_CONSULTA) cons++;
    else if(v[i].status==VISIT_ENCERRADO) { enc++; if(v[i].closed_at>0){ total_time += (long)(v[i].closed_at - v[i].opened_at); closed_count++; } }
  }
  double avg = (closed_count>0) ? (double)total_time / (double)closed_count : 0.0;

  ui_print(c.y+4,c.x+2,90,49,"Atendimentos:");
  ui_print(c.y+6,c.x+2,92,49,"• Em triagem: %d", tri);
  ui_print(c.y+7,c.x+2,93,49,"• Em consulta: %d", cons);
  ui_print(c.y+8,c.x+2,90,49,"• Encerrados: %d", enc);

  ui_print(c.y+10,c.x+2,90,49,"Tempo médio (encerrados): %.1f min", avg/60.0);
  ui_print(c.y+12,c.x+2,90,49,"(Expansão: separar por hora, fila, prioridade, risco, etc.)");

  (void)app;
}