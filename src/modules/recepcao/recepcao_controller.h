#ifndef MOD_RECEPCAO_CONTROLLER_H
#define MOD_RECEPCAO_CONTROLLER_H
#include "app/app.h"
#include "core/common.h"
#include "core/input.h"
void recepcao_controller_reset(void);
void recepcao_controller_render(App *app, Rect content);
void recepcao_controller_key(App *app, KeyEvent ev);
#endif