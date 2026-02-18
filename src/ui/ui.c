#include "ui/ui.h"

#include "core/ansi.h"
#include "core/datetime.h"
#include "ui/theme.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void ui_get_term_size(int *out_w, int *out_h) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
    *out_w = (int)ws.ws_col;
    *out_h = (int)ws.ws_row;
    return;
  }
  *out_w = 120;
  *out_h = 35;
}

void ui_init(Ui *ui) {
  memset(ui, 0, sizeof(*ui));
  ui_get_term_size(&ui->term_w, &ui->term_h);
  ui->dirty = true;
}

void ui_shutdown(Ui *ui) {
  (void)ui;
}

void ui_begin(Ui *ui) {
  ui_get_term_size(&ui->term_w, &ui->term_h);
  ansi_hide_cursor(true);
  ansi_clear();
  ansi_move(1, 1);
  ansi_set_bg(CLR_BG_APP);
  ansi_set_fg(CLR_FG_TEXT);
  fflush(stdout);
  ui->dirty = false;
}

void ui_end(Ui *ui) {
  if (ui->toast_ticks > 0) ui->toast_ticks--;
  ansi_reset();
  fflush(stdout);
  (void)ui;
}

Rect ui_layout_header(Ui *ui) {
  Rect r = {1, 1, ui->term_w, 3};
  return r;
}

Rect ui_layout_sidebar(Ui *ui) {
  Rect r = {4, 1, 28, ui->term_h - 4};
  return r;
}

Rect ui_layout_content(Ui *ui) {
  Rect r = {4, 29, ui->term_w - 29 + 1, ui->term_h - 4};
  return r;
}

void ui_draw_hline(int row, int col, int w) {
  ansi_move(row, col);
  for (int i = 0; i < w; i++) putchar('─');
}

void ui_draw_box(Rect r) {
  int x = r.x, y = r.y, w = r.w, h = r.h;
  ansi_move(y, x);
  putchar('┌');
  for (int i = 0; i < w - 2; i++) putchar('─');
  putchar('┐');

  for (int j = 1; j < h - 1; j++) {
    ansi_move(y + j, x);
    putchar('│');
    ansi_move(y + j, x + w - 1);
    putchar('│');
  }

  ansi_move(y + h - 1, x);
  putchar('└');
  for (int i = 0; i < w - 2; i++) putchar('─');
  putchar('┘');
}

void ui_print(int row, int col, int fg, int bg, const char *fmt, ...) {
  ansi_move(row, col);
  ansi_set_fg(fg);
  ansi_set_bg(bg);

  char buf[512];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  fputs(buf, stdout);
  ansi_reset();
}

void ui_toast(Ui *ui, const char *msg) {
  strncpy(ui->toast, msg, sizeof(ui->toast) - 1);
  ui->toast[sizeof(ui->toast) - 1] = '\0';
  ui->toast_ticks = 60;
  ui->dirty = true;
}