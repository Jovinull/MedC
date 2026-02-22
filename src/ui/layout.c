#include "ui/layout.h"

#include "core/ansi.h"
#include "core/datetime.h"
#include "ui/theme.h"

#include <stdio.h>
#include <string.h>

void layout_draw_header(Ui *ui, const char *app_name, const char *unit_name, const char *username) {
  Rect r = ui_layout_header(ui);
  ui_draw_box(r);
  char hhmm[6];
  dt_format_hhmm(hhmm);

  /* Row 1: app name | unit name | user info — spaced dynamically */
  int app_len = (int)strlen(app_name);
  int unit_col = r.x + 2 + app_len + 2; /* 2 chars gap after app name */

  ui_print(r.y + 1, r.x + 2, CLR_FG_ACCENT, CLR_BG_DEFAULT, "%s", app_name);
  ui_print(r.y + 1, unit_col, CLR_FG_MUTED, CLR_BG_DEFAULT, "Unidade: %s", unit_name);
  ui_print(r.y + 1, r.x + r.w - 30, CLR_FG_TEXT, CLR_BG_DEFAULT, "Usuário: %-8s %s", username, hhmm);

  /* Row 2: toast or help hint */
  if (ui->toast_ticks > 0) {
    ui_print(r.y + 2, r.x + 2, CLR_FG_OK, CLR_BG_DEFAULT, "• %s", ui->toast);
  } else {
    ui_print(r.y + 2, r.x + 2, CLR_FG_MUTED, CLR_BG_DEFAULT, "F1 Ajuda | Setas navegam | Enter abre | Esc volta | / busca");
  }
}

void layout_draw_footer(Ui *ui, const char *hint) {
  if (!hint) return;
  int row = ui->term_h;
  ui_print(row, 2, CLR_FG_MUTED, CLR_BG_DEFAULT, "%s", hint);
}