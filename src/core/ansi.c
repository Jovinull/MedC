#include "core/ansi.h"
#include <stdio.h>

void ansi_clear(void) { printf("\x1b[2J"); }
void ansi_hide_cursor(bool hide) { printf(hide ? "\x1b[?25l" : "\x1b[?25h"); }
void ansi_move(int row, int col) { printf("\x1b[%d;%dH", row, col); }
void ansi_set_fg(int color) { printf("\x1b[%dm", color); }
void ansi_set_bg(int color) { printf("\x1b[%dm", color); }
void ansi_reset(void) { printf("\x1b[0m"); }