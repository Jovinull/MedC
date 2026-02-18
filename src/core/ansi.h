#ifndef CORE_ANSI_H
#define CORE_ANSI_H

#include <stdbool.h>

void ansi_clear(void);
void ansi_hide_cursor(bool hide);
void ansi_move(int row, int col);
void ansi_set_fg(int color); /* 30-37, 90-97 */
void ansi_set_bg(int color); /* 40-47, 100-107 */
void ansi_reset(void);

#endif