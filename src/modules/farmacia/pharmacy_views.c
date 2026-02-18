#include "modules/farmacia/pharmacy_views.h"
#include "storage/repo_meds.h"
#include "storage/repo_stock.h"
#include "ui/widgets.h"
#include "ui/theme.h"
#include <stdio.h>
#include <string.h>

extern int tab;
extern int sel;

static void draw_tabs(App *app, Rect c){
  const char *t0 = tab==0 ? "[Meds]" : " Meds ";
  const char *t1 = tab==1 ? "[Lotes]" : " Lotes ";
  const char *t2 = tab==2 ? "[Relatórios]" : " Relatórios ";
  ui_print(c.y+3,c.x+2,tab==0?93:90,49,"%s",t0);
  ui_print(c.y+3,c.x+10,tab==1?93:90,49,"%s",t1);
  ui_print(c.y+3,c.x+19,tab==2?93:90,49,"%s",t2);
  ui_print(c.y+4,c.x+2,90,49,"←/→ troca aba | N novo med | I entrada | O saída manual | R saída por atendimento");
  (void)app;
}

void pharmacy_views_draw(App *app, Rect c){
  widgets_draw_table_header(&app->ui,c.y+1,c.x+2,"Farmácia • Estoque (MVP)");
  draw_tabs(app,c);

  if(tab==0){
    Med meds[128]; int n=repo_meds_list(meds,128);
    ui_print(c.y+6,c.x+2,90,49,"ID   Nome                               Un  Min  Total");
    int row=c.y+7;
    for(int i=0;i<n && i< (c.h-10); i++){
      int total = repo_stock_qty_total_for_meds(meds[i].id);
      int fg=(i==sel)?97:90;
      int bg=(i==sel)?47:49;
      ui_print(row+i,c.x+2,fg,bg,"%4d %-34s %-3s %4d %5d",
               meds[i].id,meds[i].name,meds[i].unit,meds[i].min_stock,total);
      if(total < meds[i].min_stock){
        ui_print(row+i,c.x+2+60,91,49,"↓ abaixo min");
      }
    }
    if(n==0) ui_print(c.y+9,c.x+2,90,49,"Sem medicamentos cadastrados. Pressione N.");
    return;
  }

  if(tab==1){
    StockMove mv[120]; int n=repo_stock_moves_list(mv,120);
    ui_print(c.y+6,c.x+2,90,49,"Movimentações (últimas %d)", n);
    ui_print(c.y+7,c.x+2,90,49,"ID   Med  Lote  Tipo  Qtd  Atendimento  Motivo");
    int row=c.y+8;
    for(int i=0;i<n && i< (c.h-12); i++){
      ui_print(row+i,c.x+2,90,49,"%4d %4d %4d %-4s %4d %10d  %.28s",
               mv[i].id,mv[i].meds_id,mv[i].lot_id,mv[i].type==1?"IN":"OUT",mv[i].qty,mv[i].visit_id,mv[i].reason);
    }
    if(n==0) ui_print(c.y+10,c.x+2,90,49,"Sem movimentações.");
    return;
  }

  if(tab==2){
    Med meds[128]; int n=repo_meds_list(meds,128);
    int below=0;
    for(int i=0;i<n;i++){
      int total = repo_stock_qty_total_for_meds(meds[i].id);
      if(total < meds[i].min_stock) below++;
    }
    ui_print(c.y+6,c.x+2,90,49,"Relatórios simples:");
    ui_print(c.y+8,c.x+2,90,49,"• Itens cadastrados: %d", n);
    ui_print(c.y+9,c.x+2,90,49,"• Abaixo do mínimo: %d", below);
    ui_print(c.y+11,c.x+2,90,49,"(Vencimentos por data e período de saída podem ser expandidos facilmente aqui.)");
    return;
  }
}