#ifndef MOD_TRIAGE_CONTROLLER_H
#define MOD_TRIAGE_CONTROLLER_H
#include "app/app.h"
#include "core/common.h"
#include "core/input.h"
void triage_controller_reset(void);
void triage_controller_render(App *app, Rect content);
void triage_controller_key(App *app, KeyEvent ev);
#endif
