#ifndef CORE_INPUT_H
#define CORE_INPUT_H

#include <stdbool.h>

typedef enum {
  KEY_NONE = 0,
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_ENTER,
  KEY_ESC,
  KEY_BACKSPACE,
  KEY_F1,
  KEY_CTRL_S,
  KEY_SLASH,
  KEY_CHAR
} KeyType;

typedef struct {
  KeyType type;
  char ch; /* quando type == KEY_CHAR */
} KeyEvent;

bool input_enable_raw(void);
void input_disable_raw(void);
KeyEvent input_read_key(void);

#endif