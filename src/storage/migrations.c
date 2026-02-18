#include "storage/migrations.h"

#include "core/fs.h"
#include "core/str.h"
#include "security/password.h"
#include "security/rbac.h"
#include "storage/repo_roles.h"
#include "storage/repo_users.h"
#include "storage/repo_meds.h"

#include <string.h>

void migrations_ensure_data_dir(void) {
  fs_mkdir_if_missing("data");
}

void migrations_seed_if_needed(void) {
  /* seed apenas se não existir users.dat (suficiente pro MVP) */
  if (fs_exists("data/users.dat")) return;

  repo_roles_init_store();
  repo_users_init_store();
  repo_meds_init_store();
  /* audit é criado sob demanda */

  /* Roles */
  repo_roles_seed_defaults();

  /* Admin */
  User admin = {0};
  admin.role_id = ROLE_ADMIN;
  admin.active = true;
  admin.must_change_password = true;
  str_safe_copy(admin.username, sizeof(admin.username), "admin");

  uint8_t salt[16];
  password_make_salt(salt);
  memcpy(admin.salt, salt, 16);

  char hx[65];
  password_hash("admin123", salt, hx);
  str_safe_copy(admin.password_hash_hex, sizeof(admin.password_hash_hex), hx);

  repo_users_create(&admin);

  /* Seed de alguns medicamentos */
  repo_meds_seed_defaults();
}