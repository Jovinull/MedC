#ifndef MOD_ADMIN_CONTROLLER_H
#define MOD_ADMIN_CONTROLLER_H
#include "app/app.h"
#include "core/common.h"
#include "core/input.h"
void admin_controller_reset(void);
void admin_controller_render(App *app, Rect content);
void admin_controller_key(App *app, KeyEvent ev);
#endif