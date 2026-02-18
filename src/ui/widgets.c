#include "ui/widgets.h"

#include "core/ansi.h"
#include "core/input.h"
#include "core/str.h"
#include "ui/theme.h"

#include <stdio.h>
#include <string.h>

void widgets_draw_sidebar(Ui *ui, Rect r, const char *title, const char *items[], const bool enabled[], int count, int selected) {
  ui_draw_box(r);
  ui_print(r.y + 1, r.x + 2, CLR_FG_ACCENT, CLR_BG_DEFAULT, "%s", title);

  int y = r.y + 3;
  for (int i = 0; i < count; i++) {
    int fg = enabled[i] ? CLR_FG_TEXT : CLR_FG_MUTED;
    if (i == selected) {
      ui_print(y + i, r.x + 2, fg, CLR_BG_PANEL2, "  %-22s  ", items[i]);
    } else {
      ui_print(y + i, r.x + 2, fg, CLR_BG_DEFAULT, "  %-22s  ", items[i]);
    }
  }

  (void)ui;
}

void widgets_draw_table_header(Ui *ui, int row, int col, const char *title) {
  ui_print(row, col, CLR_FG_ACCENT, CLR_BG_DEFAULT, "%s", title);
  ui_print(row + 1, col, CLR_FG_MUTED, CLR_BG_DEFAULT, "────────────────────────────────────────────────────────────────────────");
}

void widgets_draw_kv(Ui *ui, int row, int col, const char *k, const char *v) {
  ui_print(row, col, CLR_FG_MUTED, CLR_BG_DEFAULT, "%-14s", k);
  ui_print(row, col + 16, CLR_FG_TEXT, CLR_BG_DEFAULT, "%s", v ? v : "");
}

static void draw_modal(Ui *ui, Rect r, const char *title, const char *help) {
  ui_draw_box(r);
  ui_print(r.y + 1, r.x + 2, CLR_FG_ACCENT, CLR_BG_DEFAULT, "%s", title);
  if (help) ui_print(r.y + r.h - 2, r.x + 2, CLR_FG_MUTED, CLR_BG_DEFAULT, "%s", help);
  (void)ui;
}

static void field_render(Ui *ui, Rect r, int i, bool focused, const InputField *f) {
  int row = r.y + 3 + i * 2;
  ui_print(row, r.x + 2, CLR_FG_MUTED, CLR_BG_DEFAULT, "%-12s:", f->label);
  char shown[128];
  if (f->password) {
    size_t n = strlen(f->value);
    if (n >= sizeof(shown)) n = sizeof(shown) - 1;
    for (size_t k = 0; k < n; k++) shown[k] = '*';
    shown[n] = '\0';
  } else {
    str_safe_copy(shown, sizeof(shown), f->value);
  }
  ui_print(row, r.x + 16, focused ? CLR_FG_TEXT : CLR_FG_MUTED, focused ? CLR_BG_PANEL2 : CLR_BG_DEFAULT,
           " %-46s ", shown);
  (void)ui;
}

bool widgets_modal_form(Ui *ui, const char *title, InputField *fields, int nfields, const char *help) {
  Rect r = { (ui->term_w - 70)/2 + 1, (ui->term_h - (nfields*2 + 7))/2 + 1, 70, nfields*2 + 7 };
  if (r.x < 2) r.x = 2;
  if (r.y < 2) r.y = 2;

  int focus = 0;
  while (1) {
    draw_modal(ui, r, title, help);
    for (int i = 0; i < nfields; i++) field_render(ui, r, i, i == focus, &fields[i]);

    ui_print(r.y + r.h - 3, r.x + 2, CLR_FG_TEXT, CLR_BG_DEFAULT,
             "[Enter] OK   [Esc] Cancelar   [↑/↓] Campo   [Ctrl+S] Salvar");
    fflush(stdout);

    KeyEvent ev = input_read_key();
    if (ev.type == KEY_NONE) continue;

    if (ev.type == KEY_ESC) return false;
    if (ev.type == KEY_UP) { if (focus > 0) focus--; continue; }
    if (ev.type == KEY_DOWN) { if (focus < nfields - 1) focus++; continue; }
    if (ev.type == KEY_ENTER || ev.type == KEY_CTRL_S) return true;

    if (ev.type == KEY_BACKSPACE) {
      size_t n = strlen(fields[focus].value);
      if (n > 0) fields[focus].value[n - 1] = '\0';
      continue;
    }

    if (ev.type == KEY_CHAR) {
      size_t n = strlen(fields[focus].value);
      if (n + 1 < sizeof(fields[focus].value)) {
        fields[focus].value[n] = ev.ch;
        fields[focus].value[n + 1] = '\0';
      }
      continue;
    }
  }
}

bool widgets_modal_confirm(Ui *ui, const char *title, const char *msg) {
  Rect r = { (ui->term_w - 68)/2 + 1, (ui->term_h - 9)/2 + 1, 68, 9 };
  if (r.x < 2) r.x = 2;
  if (r.y < 2) r.y = 2;

  int choice = 0; /* 0=Não,1=Sim */
  while (1) {
    draw_modal(ui, r, title, "←/→ alterna | Enter confirma | Esc cancela");
    ui_print(r.y + 3, r.x + 2, CLR_FG_TEXT, CLR_BG_DEFAULT, "%s", msg);

    ui_print(r.y + 5, r.x + 2, choice ? CLR_FG_MUTED : CLR_FG_TEXT, choice ? CLR_BG_DEFAULT : CLR_BG_PANEL2, "  NÃO  ");
    ui_print(r.y + 5, r.x + 10, choice ? CLR_FG_TEXT : CLR_FG_MUTED, choice ? CLR_BG_PANEL2 : CLR_BG_DEFAULT, "  SIM  ");

    fflush(stdout);
    KeyEvent ev = input_read_key();
    if (ev.type == KEY_NONE) continue;
    if (ev.type == KEY_ESC) return false;
    if (ev.type == KEY_LEFT || ev.type == KEY_RIGHT) { choice = 1 - choice; continue; }
    if (ev.type == KEY_ENTER) return (choice == 1);
  }
}