#ifndef APP_ROUTER_H
#define APP_ROUTER_H

#include "app/app.h"
#include "core/common.h"
#include "core/input.h"

void router_on_route_change(App *app);
void router_render(App *app, Rect content);
void router_handle_key(App *app, KeyEvent ev);

#endif