#include "modules/farmacia/pharmacy_controller.h"
#include "modules/farmacia/pharmacy_views.h"
#include "storage/repo_meds.h"
#include "storage/repo_stock.h"
#include "storage/repo_rx.h"
#include "storage/repo_visits.h"
#include "storage/repo_audit.h"
#include "ui/widgets.h"
#include "core/str.h"
#include <stdlib.h>
#include <string.h>

int pharmacy_tab=0; /* 0 meds, 1 lots, 2 reports */
int pharmacy_sel=0;

void pharmacy_controller_reset(void){
  pharmacy_tab=0; pharmacy_sel=0;
  repo_meds_init_store();
  repo_stock_init_store();
  repo_rx_init_store();
  repo_visits_init_store();
}

static void med_create(App *app){
  InputField f[3];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"Nome",31);
  strncpy(f[1].label,"Unidade",31);
  strncpy(f[2].label,"Estoque mínimo",31);
  if(!widgets_modal_form(&app->ui,"Novo Medicamento",f,3,"Enter salva")) return;

  Med m; memset(&m,0,sizeof(m));
  str_safe_copy(m.name,sizeof(m.name),f[0].value);
  str_safe_copy(m.unit,sizeof(m.unit),f[1].value);
  m.min_stock=atoi(f[2].value);
  m.active=true;

  repo_meds_create(&m);
  repo_audit_log(app->session.user_id,"PHARMACY_STOCK","meds",m.id,"Medicamento cadastrado.");
  ui_toast(&app->ui,"Medicamento cadastrado.");
}

static void lot_in(App *app){
  InputField f[5];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"ID Med",31);
  strncpy(f[1].label,"Lote",31);
  strncpy(f[2].label,"Validade(YYYY-MM-DD)",31);
  strncpy(f[3].label,"Qtd",31);
  strncpy(f[4].label,"Custo unit",31);

  if(!widgets_modal_form(&app->ui,"Entrada de Estoque",f,5,"Enter registra")) return;

  int mid=atoi(f[0].value);
  Med m;
  if(!repo_meds_find_by_id(mid,&m) || !m.active){
    ui_toast(&app->ui,"Medicamento inválido.");
    return;
  }

  StockLot lot;
  memset(&lot,0,sizeof(lot));
  lot.meds_id=mid;
  str_safe_copy(lot.lot,sizeof(lot.lot),f[1].value);
  str_safe_copy(lot.expiry,sizeof(lot.expiry),f[2].value);
  lot.qty=0;
  lot.unit_cost=(float)atof(f[4].value);
  repo_stock_lot_create(&lot);

  StockMove mv;
  memset(&mv,0,sizeof(mv));
  mv.meds_id=mid;
  mv.lot_id=lot.id;
  mv.type=MOVE_IN;
  mv.qty=atoi(f[3].value);
  str_safe_copy(mv.reason,sizeof(mv.reason),"Entrada de estoque");
  mv.visit_id=0;
  repo_stock_move_create(&mv);

  repo_audit_log(app->session.user_id,"PHARMACY_IN","stock_moves",mv.id,"Entrada registrada.");
  ui_toast(&app->ui,"Entrada registrada.");
}

static void out_manual(App *app){
  InputField f[4];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"ID Med",31);
  strncpy(f[1].label,"ID Lote(opcional)",31);
  strncpy(f[2].label,"Qtd",31);
  strncpy(f[3].label,"Motivo",31);

  if(!widgets_modal_form(&app->ui,"Saída manual",f,4,"Enter registra")) return;

  int mid=atoi(f[0].value);
  int lid=atoi(f[1].value);
  int qty=atoi(f[2].value);

  if(lid>0 && repo_stock_qty_lot(lid) < qty){
    ui_toast(&app->ui,"Qtd maior que saldo do lote.");
    return;
  }

  StockMove mv;
  memset(&mv,0,sizeof(mv));
  mv.meds_id=mid;
  mv.lot_id=lid;
  mv.type=MOVE_OUT;
  mv.qty=qty;
  str_safe_copy(mv.reason,sizeof(mv.reason),f[3].value);
  mv.visit_id=0;

  repo_stock_move_create(&mv);
  repo_audit_log(app->session.user_id,"PHARMACY_OUT","stock_moves",mv.id,"Saída manual registrada.");
  ui_toast(&app->ui,"Saída registrada.");
}

static void out_by_rx(App *app){
  InputField f[3];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"ID Atendimento",31);
  strncpy(f[1].label,"ID Lote",31);
  strncpy(f[2].label,"Qtd",31);

  if(!widgets_modal_form(&app->ui,"Saída por prescrição (MVP)",f,3,"Baixa manual vinculada")) return;

  int vid=atoi(f[0].value);
  int lid=atoi(f[1].value);
  int qty=atoi(f[2].value);

  Visit v;
  if(!repo_visits_find_by_id(vid,&v)){
    ui_toast(&app->ui,"Atendimento inválido.");
    return;
  }
  StockLot lot;
  if(!repo_stock_lot_find_by_id(lid,&lot)){
    ui_toast(&app->ui,"Lote inválido.");
    return;
  }
  if(lot.qty < qty){
    ui_toast(&app->ui,"Saldo insuficiente no lote.");
    return;
  }

  StockMove mv;
  memset(&mv,0,sizeof(mv));
  mv.meds_id=lot.meds_id;
  mv.lot_id=lid;
  mv.type=MOVE_OUT;
  mv.qty=qty;
  str_safe_copy(mv.reason,sizeof(mv.reason),"Saída por prescrição/atendimento");
  mv.visit_id=vid;

  repo_stock_move_create(&mv);
  repo_audit_log(app->session.user_id,"PHARMACY_OUT_RX","stock_moves",mv.id,"Saída vinculada ao atendimento.");
  ui_toast(&app->ui,"Saída vinculada registrada.");
}

void pharmacy_controller_render(App *app, Rect c){ pharmacy_views_draw(app,c); }

void pharmacy_controller_key(App *app, KeyEvent ev){
  if(ev.type==KEY_LEFT){ if(pharmacy_tab>0) pharmacy_tab--; pharmacy_sel=0; return; }
  if(ev.type==KEY_RIGHT){ if(pharmacy_tab<2) pharmacy_tab++; pharmacy_sel=0; return; }

  if(pharmacy_tab==0){
    Med meds[128]; int n=repo_meds_list(meds,128);
    if(pharmacy_sel>=n) pharmacy_sel = n? n-1:0;
    if(ev.type==KEY_UP){ if(pharmacy_sel>0) pharmacy_sel--; return; }
    if(ev.type==KEY_DOWN){ if(pharmacy_sel<n-1) pharmacy_sel++; return; }
    if(ev.type==KEY_CHAR && (ev.ch=='n'||ev.ch=='N')){ med_create(app); return; }
    if(ev.type==KEY_CHAR && (ev.ch=='i'||ev.ch=='I')){ lot_in(app); return; }
    if(ev.type==KEY_CHAR && (ev.ch=='o'||ev.ch=='O')){ out_manual(app); return; }
    if(ev.type==KEY_CHAR && (ev.ch=='r'||ev.ch=='R')){ out_by_rx(app); return; }
    return;
  }
  if(pharmacy_tab==1){
    /* lots/moves apenas leitura neste MVP */
    if(ev.type==KEY_CHAR && (ev.ch=='i'||ev.ch=='I')){ lot_in(app); return; }
    if(ev.type==KEY_CHAR && (ev.ch=='o'||ev.ch=='O')){ out_manual(app); return; }
    if(ev.type==KEY_CHAR && (ev.ch=='r'||ev.ch=='R')){ out_by_rx(app); return; }
    return;
  }
  (void)app; (void)ev;
}