#include "storage/repo_audit.h"

#include "core/datetime.h"
#include "core/str.h"
#include "storage/db.h"

#include <stdio.h>
#include <string.h>

static const char *PATH = "data/audit.dat";

static bool ensure_header(void) {
  DBHeader h;
  if (db_load_header(PATH, &h)) return true;
  h.magic = 0x48534D56u;
  h.version = 1;
  h.next_id = 1;
  h.count = 0;
  return db_write_header(PATH, &h);
}

bool repo_audit_log(int user_id, const char *action, const char *entity, int entity_id, const char *details) {
  ensure_header();
  DBHeader h;
  if (!db_load_header(PATH, &h)) return false;

  Audit a;
  memset(&a, 0, sizeof(a));
  a.id = (int)h.next_id++;
  a.user_id = user_id;
  a.entity_id = entity_id;
  a.ts = dt_now_unix();
  str_safe_copy(a.action, sizeof(a.action), action);
  str_safe_copy(a.entity, sizeof(a.entity), entity);
  str_safe_copy(a.details, sizeof(a.details), details);

  FILE *f = fopen(PATH, "ab");
  if (!f) return false;
  if (fwrite(&a, sizeof(Audit), 1, f) != 1) { fclose(f); return false; }
  fclose(f);

  h.count++;
  return db_write_header(PATH, &h);
}

int repo_audit_list_latest(Audit *out, int cap) {
  DBHeader h;
  if (!db_load_header(PATH, &h)) return 0;

  FILE *f = fopen(PATH, "rb");
  if (!f) return 0;

  /* lê tudo e pega os últimos N (MVP simples) */
  int total = (int)h.count;
  int start = total - cap;
  if (start < 0) start = 0;

  fseek(f, (long)sizeof(DBHeader) + (long)start * (long)sizeof(Audit), SEEK_SET);

  int n = 0;
  for (int i = start; i < total && n < cap; i++) {
    if (fread(&out[n], sizeof(Audit), 1, f) != 1) break;
    n++;
  }
  fclose(f);
  return n;
}