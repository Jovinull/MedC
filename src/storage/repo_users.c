#include "storage/repo_users.h"

#include "core/datetime.h"
#include "core/str.h"
#include "storage/db.h"

#include <stdio.h>
#include <string.h>

static const char *PATH = "data/users.dat";

static bool ensure_header(void) {
  DBHeader h;
  if (db_load_header(PATH, &h)) return true;
  h.magic = 0x48534D56u;
  h.version = 1;
  h.next_id = 1;
  h.count = 0;
  return db_write_header(PATH, &h);
}

void repo_users_init_store(void) { (void)ensure_header(); }

bool repo_users_create(User *u) {
  ensure_header();
  DBHeader h;
  if (!db_load_header(PATH, &h)) return false;

  u->id = (int)h.next_id++;
  u->created_at = dt_now_unix();

  FILE *f = fopen(PATH, "ab");
  if (!f) return false;
  if (fwrite(u, sizeof(User), 1, f) != 1) { fclose(f); return false; }
  fclose(f);

  h.count++;
  return db_write_header(PATH, &h);
}

bool repo_users_update(const User *u) {
  DBHeader h;
  if (!db_load_header(PATH, &h)) return false;

  FILE *f = fopen(PATH, "r+b");
  if (!f) return false;

  fseek(f, (long)sizeof(DBHeader), SEEK_SET);
  for (uint32_t i = 0; i < h.count; i++) {
    long pos = ftell(f);
    User cur;
    if (fread(&cur, sizeof(User), 1, f) != 1) break;
    if (cur.id == u->id) {
      fseek(f, pos, SEEK_SET);
      if (fwrite(u, sizeof(User), 1, f) != 1) { fclose(f); return false; }
      fclose(f);
      return true;
    }
  }
  fclose(f);
  return false;
}

bool repo_users_find_by_username(const char *username, User *out) {
  DBHeader h;
  if (!db_load_header(PATH, &h)) return false;
  FILE *f = fopen(PATH, "rb");
  if (!f) return false;

  fseek(f, (long)sizeof(DBHeader), SEEK_SET);
  for (uint32_t i = 0; i < h.count; i++) {
    User u;
    if (fread(&u, sizeof(User), 1, f) != 1) break;
    if (str_eq(u.username, username)) {
      *out = u;
      fclose(f);
      return true;
    }
  }
  fclose(f);
  return false;
}

bool repo_users_find_by_id(int id, User *out) {
  DBHeader h;
  if (!db_load_header(PATH, &h)) return false;
  FILE *f = fopen(PATH, "rb");
  if (!f) return false;

  fseek(f, (long)sizeof(DBHeader), SEEK_SET);
  for (uint32_t i = 0; i < h.count; i++) {
    User u;
    if (fread(&u, sizeof(User), 1, f) != 1) break;
    if (u.id == id) {
      *out = u;
      fclose(f);
      return true;
    }
  }
  fclose(f);
  return false;
}

int repo_users_list(User *out, int cap) {
  DBHeader h;
  if (!db_load_header(PATH, &h)) return 0;

  FILE *f = fopen(PATH, "rb");
  if (!f) return 0;

  fseek(f, (long)sizeof(DBHeader), SEEK_SET);
  int n = 0;
  for (uint32_t i = 0; i < h.count && n < cap; i++) {
    if (fread(&out[n], sizeof(User), 1, f) != 1) break;
    n++;
  }
  fclose(f);
  return n;
}