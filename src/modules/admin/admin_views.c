#include "modules/admin/admin_views.h"
#include "storage/repo_users.h"
#include "storage/repo_audit.h"
#include "security/rbac.h"
#include "core/datetime.h"
#include "ui/widgets.h"
#include "ui/theme.h"
#include <stdio.h>

extern int admin_tab;
extern int admin_sel;

static void draw_tabs(App *app, Rect c){
  const char *t0 = admin_tab==0 ? "[Usuários]" : " Usuários ";
  const char *t1 = admin_tab==1 ? "[Auditoria]" : " Auditoria ";
  ui_print(c.y+3,c.x+2,admin_tab==0?93:90,49,"%s",t0);
  ui_print(c.y+3,c.x+14,admin_tab==1?93:90,49,"%s",t1);
  ui_print(c.y+4,c.x+2,90,49,"←/→ troca aba | N novo usuário | Enter alterna ativo/inativo (usuários)");
  (void)app;
}

void admin_views_draw(App *app, Rect c){
  widgets_draw_table_header(&app->ui,c.y+1,c.x+2,"Administração (MVP)");
  draw_tabs(app,c);

  if(admin_tab==0){
    User users[128]; int n=repo_users_list(users,128);
    ui_print(c.y+6,c.x+2,90,49,"ID   Username                 Role       Ativo  TrocaSenha");
    int row=c.y+7;
    for(int i=0;i<n && i< (c.h-10); i++){
      int fg=(i==admin_sel)?97:90;
      int bg=(i==admin_sel)?47:49;
      ui_print(row+i,c.x+2,fg,bg,"%4d %-22s %-10s %5s %10s",
               users[i].id, users[i].username, rbac_role_name(users[i].role_id),
               users[i].active?"SIM":"NAO", users[i].must_change_password?"SIM":"NAO");
    }
    return;
  }

  if(admin_tab==1){
    Audit a[80]; int n=repo_audit_list_latest(a,80);
    ui_print(c.y+6,c.x+2,90,49,"Últimos eventos de auditoria:");
    ui_print(c.y+7,c.x+2,90,49,"TS                  User  Ação            Entidade   ID   Detalhes");
    int row=c.y+8;
    for(int i=0;i<n && i< (c.h-12); i++){
      char dt[32]; dt_format_ymd_hms(dt,a[i].ts);
      ui_print(row+i,c.x+2,90,49,"%-19s %4d %-14s %-9s %4d %.34s",
               dt,a[i].user_id,a[i].action,a[i].entity,a[i].entity_id,a[i].details);
    }
    return;
  }
}