#include "app/app.h"

#include "app/router.h"
#include "core/input.h"
#include "security/rbac.h"
#include "ui/layout.h"
#include "ui/widgets.h"

#include <string.h>

static const char *APP_NAME = "SmartHosp TUI (MVP)";
static const char *UNIT_NAME = "Hospital Demo";

typedef struct {
  const char *label;
  int route;
  Permission perm; /* 0 = público/autenticado */
} NavItem;

enum {
  ROUTE_HOME = 1,
  ROUTE_CAD_PACIENTES,
  ROUTE_RECEPCAO,
  ROUTE_TRIAGEM,
  ROUTE_CONSULTORIO,
  ROUTE_FARMACIA,
  ROUTE_INDICADORES,
  ROUTE_ADMIN
};

static NavItem NAV[] = {
  {"Home", ROUTE_HOME, 0},
  {"Recepção", ROUTE_RECEPCAO, PERM_CHECKIN_CREATE},
  {"Classificação", ROUTE_TRIAGEM, PERM_TRIAGE_WRITE},
  {"Consultório", ROUTE_CONSULTORIO, PERM_CONSULT_WRITE},
  {"Farmácia", ROUTE_FARMACIA, PERM_PHARMACY_STOCK},
  {"Cadastros", ROUTE_CAD_PACIENTES, PERM_PATIENT_CRUD},
  {"Indicadores", ROUTE_INDICADORES, PERM_REPORT_VIEW},
  {"Admin/Perfis", ROUTE_ADMIN, PERM_USER_MANAGE},
};

static void draw_shell(App *app) {
  Ui *ui = &app->ui;
  layout_draw_header(ui, APP_NAME, UNIT_NAME,
                     app->session.authenticated ? app->session.username : "(offline)");

  Rect sidebar = ui_layout_sidebar(ui);
  Rect content = ui_layout_content(ui);

  const int n = (int)(sizeof(NAV)/sizeof(NAV[0]));
  const char *labels[16];
  bool enabled[16];
  for (int i=0; i<n; i++) {
    labels[i] = NAV[i].label;
    enabled[i] = app->session.authenticated &&
                 (NAV[i].perm == 0 || rbac_has(app->session.perms, NAV[i].perm));
  }

  widgets_draw_sidebar(ui, sidebar, "Menu", labels, enabled, n, app->sidebar_selected);
  ui_draw_box(content);
}

static void force_password_change(App *app) {
  if (!app->session.must_change_password) return;

  InputField f[2];
  memset(f, 0, sizeof(f));
  strncpy(f[0].label, "Senha atual", sizeof(f[0].label)-1);
  f[0].password = true;
  strncpy(f[1].label, "Nova senha", sizeof(f[1].label)-1);
  f[1].password = true;

  while (app->session.must_change_password) {
    ui_begin(&app->ui);
    draw_shell(app);
    ui_end(&app->ui);

    bool ok = widgets_modal_form(&app->ui, "Troca obrigatória de senha", f, 2,
                                 "Por segurança, altere a senha no primeiro login.");
    if (!ok) continue;

    char err[96];
    if (!auth_change_password(&app->session, f[0].value, f[1].value, err)) {
      ui_toast(&app->ui, err);
      memset(f[0].value, 0, sizeof(f[0].value));
      memset(f[1].value, 0, sizeof(f[1].value));
      continue;
    }
    ui_toast(&app->ui, "Senha alterada com sucesso.");
  }
}

static void login_flow(App *app) {
  InputField f[2];
  memset(f, 0, sizeof(f));
  strncpy(f[0].label, "Usuário", sizeof(f[0].label)-1);
  strncpy(f[1].label, "Senha", sizeof(f[1].label)-1);
  f[1].password = true;

  while (!app->session.authenticated) {
    ui_begin(&app->ui);
    layout_draw_header(&app->ui, APP_NAME, UNIT_NAME, "(login)");
    Rect c = ui_layout_content(&app->ui);
    ui_draw_box(c);
    ui_print(c.y+2, c.x+2, 90, 49, "Acesso ao sistema");
    ui_print(c.y+4, c.x+2, 90, 49, "Use: admin / admin123 (primeiro login exige troca).");
    ui_end(&app->ui);

    bool ok = widgets_modal_form(&app->ui, "Login", f, 2, "Enter confirma | Esc fecha");
    if (!ok) continue;

    char err[96];
    if (!auth_login(&app->session, f[0].value, f[1].value, err)) {
      ui_toast(&app->ui, err);
      memset(f[1].value, 0, sizeof(f[1].value));
      continue;
    }
    ui_toast(&app->ui, "Login efetuado.");
  }
}

void app_init(App *app) {
  memset(app, 0, sizeof(*app));
  ui_init(&app->ui);
  session_clear(&app->session);
  app->sidebar_selected = 0;
  app->current_route = ROUTE_HOME;
}

void app_shutdown(App *app) {
  input_disable_raw();
  ui_shutdown(&app->ui);
}

void app_run(App *app) {
  input_enable_raw();

  login_flow(app);
  force_password_change(app);

  const int n = (int)(sizeof(NAV)/sizeof(NAV[0]));

  while (1) {
    ui_begin(&app->ui);
    draw_shell(app);

    Rect content = ui_layout_content(&app->ui);
    router_render(app, content);
    ui_end(&app->ui);

    KeyEvent ev = input_read_key();
    if (ev.type == KEY_NONE) continue;

    if (ev.type == KEY_F1) {
      ui_toast(&app->ui, "Ajuda: ↑↓ navega | Enter abre | Esc volta | / busca em listas.");
      continue;
    }

    if (ev.type == KEY_ESC) {
      /* sair do app com confirmação */
      if (widgets_modal_confirm(&app->ui, "Sair", "Deseja sair do sistema?")) break;
      continue;
    }

    if (ev.type == KEY_UP) { if (app->sidebar_selected > 0) app->sidebar_selected--; continue; }
    if (ev.type == KEY_DOWN) { if (app->sidebar_selected < n-1) app->sidebar_selected++; continue; }

    if (ev.type == KEY_ENTER) {
      NavItem it = NAV[app->sidebar_selected];
      bool ok = (it.perm == 0) || rbac_has(app->session.perms, it.perm);
      if (!ok) {
        ui_toast(&app->ui, "Acesso negado para este perfil.");
        continue;
      }
      app->current_route = it.route;
      router_on_route_change(app);
      continue;
    }

    /* repassa teclas para a rota atual */
    router_handle_key(app, ev);
  }
}