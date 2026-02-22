#include "ui/ui.h"

#include "core/ansi.h"
#include "core/datetime.h"
#include "ui/theme.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
void ui_get_term_size(int *out_w, int *out_h) {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    *out_w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *out_h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return;
  }
  *out_w = 120;
  *out_h = 35;
}
#else
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
#endif

static char s_stdout_buf[1 << 16]; /* 64 KiB buffer for flicker-free output */

void ui_init(Ui *ui) {
  memset(ui, 0, sizeof(*ui));
#if defined(_WIN32)
  /* Enable ANSI escape sequences and UTF-8 output on Windows console */
  {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    mode |= 0x0004; /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
    SetConsoleMode(hOut, mode);
    SetConsoleOutputCP(65001);
  }
#endif
  /* Full-buffered stdout: everything accumulates until fflush in ui_end.
     This eliminates flicker — the terminal sees one big write per frame. */
  setvbuf(stdout, s_stdout_buf, _IOFBF, sizeof(s_stdout_buf));
  /* Enter alternate screen buffer so original terminal is restored on exit */
  printf("\x1b[?1049h");
  /* Disable auto-wrap — text that exceeds the right edge is clipped,
     preventing long lines from wrapping into adjacent UI panels */
  printf("\x1b[?7l");
  fflush(stdout);
  ui_get_term_size(&ui->term_w, &ui->term_h);
  ui->dirty = true;
}

void ui_shutdown(Ui *ui) {
  (void)ui;
  ansi_hide_cursor(false);
  ansi_reset();
  /* Re-enable auto-wrap and leave alternate screen buffer */
  printf("\x1b[?7h");
  printf("\x1b[?1049l");
  fflush(stdout);
  /* Restore line-buffered stdout */
  setvbuf(stdout, NULL, _IOLBF, 0);
}

void ui_begin(Ui *ui) {
  ui_get_term_size(&ui->term_w, &ui->term_h);
  ansi_hide_cursor(true);
  /* Full-screen clear is fine because stdout is fully-buffered.
     It goes into the buffer along with all draw calls and only appears
     on screen when ui_end flushes — so there is no visible flash. */
  ansi_clear();
  ansi_move(1, 1);
  ansi_set_bg(CLR_BG_APP);
  ansi_set_fg(CLR_FG_TEXT);
  /* Do NOT fflush here — let everything buffer until ui_end */
  ui->dirty = false;
}

void ui_end(Ui *ui) {
  if (ui->toast_ticks > 0) ui->toast_ticks--;
  ansi_reset();
  fflush(stdout);
  (void)ui;
}

Rect ui_layout_header(Ui *ui) {
  /* Rect = {x(col), y(row), w, h} */
  Rect r = {1, 1, ui->term_w, 3};
  return r;
}

Rect ui_layout_sidebar(Ui *ui) {
  /* col=1, row=4 (below 3-row header), width=28 */
  Rect r = {1, 4, 28, ui->term_h - 4};
  return r;
}

Rect ui_layout_content(Ui *ui) {
  /* col=29 (after 28-col sidebar), row=4 */
  Rect r = {29, 4, ui->term_w - 29 + 1, ui->term_h - 4};
  return r;
}

void ui_draw_hline(int row, int col, int w) {
  ansi_move(row, col);
  for (int i = 0; i < w; i++) fputs("\xe2\x94\x80", stdout); /* ─ */
}

void ui_draw_box(Rect r) {
  int x = r.x, y = r.y, w = r.w, h = r.h;
  ansi_move(y, x);
  fputs("\xe2\x94\x8c", stdout); /* ┌ */
  for (int i = 0; i < w - 2; i++) fputs("\xe2\x94\x80", stdout); /* ─ */
  fputs("\xe2\x94\x90", stdout); /* ┐ */

  for (int j = 1; j < h - 1; j++) {
    ansi_move(y + j, x);
    fputs("\xe2\x94\x82", stdout); /* │ */
    ansi_move(y + j, x + w - 1);
    fputs("\xe2\x94\x82", stdout); /* │ */
  }

  ansi_move(y + h - 1, x);
  fputs("\xe2\x94\x94", stdout); /* └ */
  for (int i = 0; i < w - 2; i++) fputs("\xe2\x94\x80", stdout); /* ─ */
  fputs("\xe2\x94\x98", stdout); /* ┘ */
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