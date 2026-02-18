#ifndef MOD_INDICATORS_CONTROLLER_H
#define MOD_INDICATORS_CONTROLLER_H
#include "app/app.h"
#include "core/common.h"
#include "core/input.h"
void indicators_controller_reset(void);
void indicators_controller_render(App *app, Rect content);
void indicators_controller_key(App *app, KeyEvent ev);
#endif