#include "modules/indicadores/indicators_controller.h"
#include "modules/indicadores/indicators_views.h"
#include "storage/repo_visits.h"

void indicators_controller_reset(void){
  repo_visits_init_store();
}

void indicators_controller_render(App *app, Rect c){ indicators_views_draw(app,c); }

void indicators_controller_key(App *app, KeyEvent ev){
  (void)app; (void)ev;
}
