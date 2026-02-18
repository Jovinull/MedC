#include "modules/admin/admin_controller.h"
#include "modules/admin/admin_views.h"
#include "storage/repo_users.h"
#include "storage/repo_roles.h"
#include "storage/repo_audit.h"
#include "security/password.h"
#include "security/rbac.h"
#include "ui/widgets.h"
#include "core/str.h"
#include <string.h>

static int tab=0; /* 0 users, 1 audit */
static int sel=0;

void admin_controller_reset(void){
  tab=0; sel=0;
  repo_users_init_store();
  repo_roles_init_store();
}

static void user_create(App *app){
  InputField f[4];
  memset(f,0,sizeof(f));
  strncpy(f[0].label,"Username",31);
  strncpy(f[1].label,"Senha",31); f[1].password=true;
  strncpy(f[2].label,"Role(1-5)",31);
  strncpy(f[3].label,"Ativo(1/0)",31);
  str_safe_copy(f[2].value,sizeof(f[2].value),"2");
  str_safe_copy(f[3].value,sizeof(f[3].value),"1");

  if(!widgets_modal_form(&app->ui,"Admin • Novo usuário",f,4,"Roles: 1 ADMIN,2 ATEND,3 ENF,4 MED,5 FARM")) return;

  User u; memset(&u,0,sizeof(u));
  str_safe_copy(u.username,sizeof(u.username),f[0].value);
  u.role_id = atoi(f[2].value);
  u.active = atoi(f[3].value) ? true:false;
  u.must_change_password = true;

  uint8_t salt[16]; password_make_salt(salt); memcpy(u.salt,salt,16);
  char hx[65]; password_hash(f[1].value,salt,hx);
  str_safe_copy(u.password_hash_hex,sizeof(u.password_hash_hex),hx);

  if(strlen(u.username)<3){
    ui_toast(&app->ui,"Username inválido.");
    return;
  }

  repo_users_create(&u);
  repo_audit_log(app->session.user_id,"USER_MANAGE","users",u.id,"Usuário criado.");
  ui_toast(&app->ui,"Usuário criado (troca de senha obrigatória no 1º login).");
}

static void toggle_active(App *app, User *u){
  if(!widgets_modal_confirm(&app->ui,"Admin","Alternar ativo/inativo?")) return;
  u->active = !u->active;
  repo_users_update(u);
  repo_audit_log(app->session.user_id,"USER_MANAGE","users",u->id,u->active?"Reativado":"Desativado");
  ui_toast(&app->ui,"Atualizado.");
}

void admin_controller_render(App *app, Rect c){ admin_views_draw(app,c); }

void admin_controller_key(App *app, KeyEvent ev){
  if(ev.type==KEY_LEFT){ if(tab>0) tab--; sel=0; return; }
  if(ev.type==KEY_RIGHT){ if(tab<1) tab++; sel=0; return; }

  if(tab==0){
    User users[128]; int n=repo_users_list(users,128);
    if(sel>=n) sel = n? n-1:0;
    if(ev.type==KEY_UP){ if(sel>0) sel--; return; }
    if(ev.type==KEY_DOWN){ if(sel<n-1) sel++; return; }
    if(ev.type==KEY_CHAR && (ev.ch=='n'||ev.ch=='N')){ user_create(app); return; }
    if(ev.type==KEY_ENTER && n>0){
      User u = users[sel];
      if(u.id==app->session.user_id){
        ui_toast(&app->ui,"Você não pode desativar seu próprio usuário.");
        return;
      }
      toggle_active(app,&u);
      return;
    }
    return;
  }
  (void)app; (void)ev;
}