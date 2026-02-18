#ifndef MOD_PHARMACY_CONTROLLER_H
#define MOD_PHARMACY_CONTROLLER_H
#include "app/app.h"
#include "core/common.h"
#include "core/input.h"
void pharmacy_controller_reset(void);
void pharmacy_controller_render(App *app, Rect content);
void pharmacy_controller_key(App *app, KeyEvent ev);
#endif