#ifndef MOD_CONSULT_CONTROLLER_H
#define MOD_CONSULT_CONTROLLER_H
#include "app/app.h"
#include "core/common.h"
#include "core/input.h"
void consult_controller_reset(void);
void consult_controller_render(App *app, Rect content);
void consult_controller_key(App *app, KeyEvent ev);
#endif