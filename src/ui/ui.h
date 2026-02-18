#ifndef UI_UI_H
#define UI_UI_H

#include "core/common.h"
#include "core/input.h"
#include <stdbool.h>

typedef struct {
  int term_w;
  int term_h;
  bool dirty;
  char toast[96];
  int toast_ticks;
} Ui;

void ui_init(Ui *ui);
void ui_shutdown(Ui *ui);

void ui_begin(Ui *ui);
void ui_end(Ui *ui);

Rect ui_layout_header(Ui *ui);
Rect ui_layout_sidebar(Ui *ui);
Rect ui_layout_content(Ui *ui);

void ui_draw_box(Rect r);
void ui_draw_hline(int row, int col, int w);
void ui_print(int row, int col, int fg, int bg, const char *fmt, ...);

void ui_toast(Ui *ui, const char *msg);

void ui_get_term_size(int *out_w, int *out_h);

#endif