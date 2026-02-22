#include "app/router.h"

#include "modules/admin/admin_controller.h"
#include "modules/cadastros/patients_controller.h"
#include "modules/consultorio/consult_controller.h"
#include "modules/enfermagem/triage_controller.h"
#include "modules/farmacia/pharmacy_controller.h"
#include "modules/indicadores/indicators_controller.h"
#include "modules/recepcao/recepcao_controller.h"
#include "ui/widgets.h"
#include "ui/theme.h"

static void render_home(App *app, Rect c) {
  (void)app;
  widgets_draw_table_header(&app->ui, c.y + 1, c.x + 2, "Home");
  ui_print(c.y + 3, c.x + 2, CLR_FG_TEXT, 49, "Bem-vindo ao MVP de Gestão Hospitalar (TUI).");
  ui_print(c.y + 5, c.x + 2, CLR_FG_MUTED, 49, "Módulos implementados: Pacientes, Check-in, Triagem, Consultório, Farmácia, Indicadores, Admin.");
  ui_print(c.y + 7, c.x + 2, CLR_FG_MUTED, 49, "Tudo é local e rápido. Expansão futura: exames/laudos completos, ambulatório, enfermaria detalhada, etc.");
}

void router_on_route_change(App *app) {
  (void)app;
  /* cada controller pode resetar estado interno */
  patients_controller_reset();
  recepcao_controller_reset();
  triage_controller_reset();
  consult_controller_reset();
  pharmacy_controller_reset();
  indicators_controller_reset();
  admin_controller_reset();
}

void router_render(App *app, Rect content) {
  switch (app->current_route) {
    case 1: render_home(app, content); break;
    case 2: patients_controller_render(app, content); break;
    case 3: recepcao_controller_render(app, content); break;
    case 4: triage_controller_render(app, content); break;
    case 5: consult_controller_render(app, content); break;
    case 6: pharmacy_controller_render(app, content); break;
    case 7: indicators_controller_render(app, content); break;
    case 8: admin_controller_render(app, content); break;
    default: render_home(app, content); break;
  }
}

void router_handle_key(App *app, KeyEvent ev) {
  switch (app->current_route) {
    case 2: patients_controller_key(app, ev); break;
    case 3: recepcao_controller_key(app, ev); break;
    case 4: triage_controller_key(app, ev); break;
    case 5: consult_controller_key(app, ev); break;
    case 6: pharmacy_controller_key(app, ev); break;
    case 7: indicators_controller_key(app, ev); break;
    case 8: admin_controller_key(app, ev); break;
    default: (void)app; (void)ev; break;
  }
}