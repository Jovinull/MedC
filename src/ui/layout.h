#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include "core/common.h"
#include "ui/ui.h"

void layout_draw_header(Ui *ui, const char *app_name, const char *unit_name, const char *username);
void layout_draw_footer(Ui *ui, const char *hint);

#endif