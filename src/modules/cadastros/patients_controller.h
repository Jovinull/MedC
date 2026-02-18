#ifndef MOD_PATIENTS_CONTROLLER_H
#define MOD_PATIENTS_CONTROLLER_H
#include "app/app.h"
#include "core/common.h"
#include "core/input.h"
void patients_controller_reset(void);
void patients_controller_render(App *app, Rect content);
void patients_controller_key(App *app, KeyEvent ev);
#endif