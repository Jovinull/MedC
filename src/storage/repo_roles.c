#include "storage/repo_roles.h"
#include "core/str.h"
#include "storage/db.h"

#include <stdio.h>
#include <string.h>

static const char *PATH = "data/roles.dat";

static bool ensure_header(void) {
  DBHeader h;
  if (db_load_header(PATH, &h)) return true;
  h.magic = 0x48534D56u;
  h.version = 1;
  h.next_id = 1;
  h.count = 0;
  return db_write_header(PATH, &h);
}

void repo_roles_init_store(void) { (void)ensure_header(); }

static bool append_role(Role *r) {
  DBHeader h;
  if (!db_load_header(PATH, &h)) return false;
  r->id = (int)h.next_id++;

  FILE *f = fopen(PATH, "ab");
  if (!f) return false;
  if (fwrite(r, sizeof(Role), 1, f) != 1) { fclose(f); return false; }
  fclose(f);

  h.count++;
  return db_write_header(PATH, &h);
}

void repo_roles_seed_defaults(void) {
  ensure_header();

  Role r;
  memset(&r, 0, sizeof(r));
  str_safe_copy(r.name, sizeof(r.name), "ADMIN");      append_role(&r);
  str_safe_copy(r.name, sizeof(r.name), "ATENDENTE");  append_role(&r);
  str_safe_copy(r.name, sizeof(r.name), "ENFERMAGEM"); append_role(&r);
  str_safe_copy(r.name, sizeof(r.name), "MEDICO");     append_role(&r);
  str_safe_copy(r.name, sizeof(r.name), "FARMACIA");   append_role(&r);
}

bool repo_roles_find_by_id(int id, Role *out) {
  DBHeader h;
  if (!db_load_header(PATH, &h)) return false;
  FILE *f = fopen(PATH, "rb");
  if (!f) return false;

  fseek(f, (long)sizeof(DBHeader), SEEK_SET);
  for (uint32_t i = 0; i < h.count; i++) {
    Role r;
    if (fread(&r, sizeof(Role), 1, f) != 1) break;
    if (r.id == id) {
      *out = r;
      fclose(f);
      return true;
    }
  }
  fclose(f);
  return false;
}