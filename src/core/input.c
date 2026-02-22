#include "core/input.h"

#include <stdio.h>

/* ================================================================== */
/*  Windows implementation using Win32 Console API                     */
/* ================================================================== */
#if defined(_WIN32)

#include <conio.h>
#include <windows.h>

static DWORD g_old_mode = 0;
static HANDLE g_hin     = INVALID_HANDLE_VALUE;
static bool   g_raw     = false;

bool input_enable_raw(void) {
  if (g_raw) return true;
  g_hin = GetStdHandle(STD_INPUT_HANDLE);
  if (g_hin == INVALID_HANDLE_VALUE) return false;

  if (!GetConsoleMode(g_hin, &g_old_mode)) return false;

  DWORD mode = g_old_mode;
  mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
  if (!SetConsoleMode(g_hin, mode)) return false;

  /* Enable UTF-8 output so box-drawing chars display correctly */
  SetConsoleOutputCP(65001);

  g_raw = true;
  return true;
}

void input_disable_raw(void) {
  if (!g_raw) return;
  SetConsoleMode(g_hin, g_old_mode);
  g_raw = false;
}

KeyEvent input_read_key(void) {
  KeyEvent e = {KEY_NONE, 0};

  if (!_kbhit()) return e;

  int c = _getch();

  /* Arrow keys and F-keys come as 0x00 or 0xE0 prefix + second byte */
  if (c == 0x00 || c == 0xE0) {
    int c2 = _getch();
    switch (c2) {
      case 72: e.type = KEY_UP;    return e;
      case 80: e.type = KEY_DOWN;  return e;
      case 75: e.type = KEY_LEFT;  return e;
      case 77: e.type = KEY_RIGHT; return e;
      case 59: e.type = KEY_F1;    return e;
      default: return e;
    }
  }

  if (c == 27)             { e.type = KEY_ESC;       return e; }
  if (c == '\r' || c == '\n') { e.type = KEY_ENTER;  return e; }
  if (c == 8)              { e.type = KEY_BACKSPACE;  return e; }
  if (c == 19)             { e.type = KEY_CTRL_S;     return e; } /* Ctrl+S */
  if (c == '/')            { e.type = KEY_SLASH;      return e; }

  if (c >= 32 && c <= 126) {
    e.type = KEY_CHAR;
    e.ch   = (char)c;
    return e;
  }

  return e;
}

/* ================================================================== */
/*  POSIX / Linux implementation using termios                         */
/* ================================================================== */
#else

#include <unistd.h>
#include <termios.h>

static struct termios g_old;
static bool g_raw = false;

bool input_enable_raw(void) {
  if (g_raw) return true;
  if (tcgetattr(STDIN_FILENO, &g_old) == -1) return false;

  struct termios raw = g_old;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN]  = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) return false;
  g_raw = true;
  return true;
}

void input_disable_raw(void) {
  if (!g_raw) return;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_old);
  g_raw = false;
}

static int read_byte(void) {
  unsigned char c;
  int n = (int)read(STDIN_FILENO, &c, 1);
  if (n == 1) return (int)c;
  return -1;
}

KeyEvent input_read_key(void) {
  KeyEvent e = {KEY_NONE, 0};

  int c = read_byte();
  if (c < 0) return e;

  if (c == 27) {
    int c2 = read_byte();
    if (c2 < 0) { e.type = KEY_ESC; return e; }
    if (c2 == '[') {
      int c3 = read_byte();
      if (c3 == 'A') e.type = KEY_UP;
      else if (c3 == 'B') e.type = KEY_DOWN;
      else if (c3 == 'C') e.type = KEY_RIGHT;
      else if (c3 == 'D') e.type = KEY_LEFT;
      else e.type = KEY_NONE;
      return e;
    }
    if (c2 == 'O') {
      int c3 = read_byte();
      if (c3 == 'P') e.type = KEY_F1;
      return e;
    }
    e.type = KEY_ESC;
    return e;
  }

  if (c == '\r' || c == '\n') { e.type = KEY_ENTER;     return e; }
  if (c == 127 || c == 8)     { e.type = KEY_BACKSPACE;  return e; }
  if (c == 19)                 { e.type = KEY_CTRL_S;     return e; } /* Ctrl+S */
  if (c == '/')                { e.type = KEY_SLASH;      return e; }

  if (c >= 32 && c <= 126) {
    e.type = KEY_CHAR;
    e.ch   = (char)c;
    return e;
  }

  return e;
}

#endif