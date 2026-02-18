#ifndef APP_APP_H
#define APP_APP_H

#include "app/session.h"
#include "ui/ui.h"

typedef struct {
  Ui ui;
  Session session;

  int sidebar_selected;
  int current_route;
} App;

void app_init(App *app);
void app_run(App *app);
void app_shutdown(App *app);

#endif