#include "modules/cadastros/patients_views.h"
#include "storage/repo_patients.h"
#include "core/datetime.h"
#include "core/str.h"
#include "ui/widgets.h"
#include "ui/theme.h"
#include <string.h>
#include <stdio.h>

extern int sel;
extern char filter[32];

void patients_views_draw(App *app, Rect c){
  widgets_draw_table_header(&app->ui,c.y+1,c.x+2,"Cadastros • Pacientes");
  ui_print(c.y+3,c.x+2,90,49,"[N] Novo   [Enter] Editar   [/ ] Buscar");

  Patient list[256];
  int n=repo_patients_list(list,256);

  int idxs[256]; int k=0;
  for(int i=0;i<n;i++){
    if(!list[i].active) continue;
    if(filter[0]==0 || str_has(list[i].name,filter) || str_has(list[i].cpf,filter)) idxs[k++]=i;
  }

  ui_print(c.y+5,c.x+2,90,49,"Filtro: %s", filter[0]?filter:"(nenhum)");
  ui_print(c.y+6,c.x+2,90,49,"ID   Nome                           CPF            Nasc       Tel");

  int row=c.y+7;
  for(int i=0;i<k && i< (c.h-9); i++){
    Patient *p=&list[idxs[i]];
    int fg = (i==sel) ? 97 : 90;
    int bg = (i==sel) ? 47 : 49;
    ui_print(row+i,c.x+2,fg,bg,"%4d %-30s %-13s %-10s %-14s",
             p->id,p->name,p->cpf,p->birth_date,p->phone);
  }
  if(k==0){
    ui_print(c.y+9,c.x+2,90,49,"Nenhum paciente encontrado. Pressione N para cadastrar.");
  }
  (void)app;
}