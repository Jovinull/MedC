#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include "core/common.h"
#include "core/input.h"
#include "ui/ui.h"
#include <stdbool.h>

typedef struct {
  int selected;
  int count;
} ListState;

void widgets_draw_sidebar(Ui *ui, Rect r, const char *title, const char *items[], const bool enabled[], int count, int selected);

void widgets_draw_table_header(Ui *ui, int row, int col, const char *title);
void widgets_draw_kv(Ui *ui, int row, int col, const char *k, const char *v);

typedef struct {
  char label[32];
  char value[128];
  bool password;
} InputField;

bool widgets_modal_form(Ui *ui, const char *title, InputField *fields, int nfields, const char *help);

bool widgets_modal_confirm(Ui *ui, const char *title, const char *msg);

#endif