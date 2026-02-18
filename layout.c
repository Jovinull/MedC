#include "ui/layout.h"

#include "core/datetime.h"
#include "ui/theme.h"

#include <string.h>

void layout_draw_header(Ui *ui, const char *app_name, const char *unit_name, const char *username) {
  Rect r = ui_layout_header(ui);
  ui_draw_box((Rect){r.x, r.y, r.w, r.h});
  char hhmm[6];
  dt_format_hhmm(hhmm);

  ui_print(r.y + 1, r.x + 2, CLR_FG_ACCENT, CLR_BG_DEFAULT, "%s", app_name);
  ui_print(r.y + 1, r.x + 18, CLR_FG_MUTED, CLR_BG_DEFAULT, "Unidade: %s", unit_name);
  ui_print(r.y + 1, r.x + r.w - 30, CLR_FG_TEXT, CLR_BG_DEFAULT, "Usuário: %s  %s", username, hhmm);

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