#include "security/auth.h"

#include "core/str.h"
#include "security/password.h"
#include "security/rbac.h"
#include "storage/repo_audit.h"
#include "storage/repo_users.h"

#include <string.h>

bool auth_login(Session *s, const char *username, const char *password, char err[96]) {
  err[0] = '\0';
  User u;
  if (!repo_users_find_by_username(username, &u)) {
    str_safe_copy(err, 96, "Usuário ou senha inválidos.");
    return false;
  }
  if (!u.active) {
    str_safe_copy(err, 96, "Usuário desativado.");
    return false;
  }
  if (!password_verify(password, u.salt, u.password_hash_hex)) {
    str_safe_copy(err, 96, "Usuário ou senha inválidos.");
    return false;
  }

  session_clear(s);
  s->authenticated = true;
  s->user_id = u.id;
  s->role_id = u.role_id;
  s->perms = rbac_permissions_for_role(u.role_id);
  str_safe_copy(s->username, sizeof(s->username), u.username);
  session_make_token(s->token);
  s->must_change_password = u.must_change_password;

  repo_audit_log(u.id, "LOGIN", "users", u.id, "Login realizado com sucesso.");
  return true;
}

bool auth_change_password(Session *s, const char *oldpw, const char *newpw, char err[96]) {
  err[0] = '\0';
  if (!s->authenticated) {
    str_safe_copy(err, 96, "Sessão inválida.");
    return false;
  }

  User u;
  if (!repo_users_find_by_id(s->user_id, &u)) {
    str_safe_copy(err, 96, "Usuário não encontrado.");
    return false;
  }

  if (!password_verify(oldpw, u.salt, u.password_hash_hex)) {
    str_safe_copy(err, 96, "Senha atual incorreta.");
    return false;
  }

  uint8_t salt[16];
  password_make_salt(salt);

  char hx[65];
  password_hash(newpw, salt, hx);

  memcpy(u.salt, salt, 16);
  str_safe_copy(u.password_hash_hex, sizeof(u.password_hash_hex), hx);
  u.must_change_password = false;

  repo_users_update(&u);
  repo_audit_log(u.id, "PASSWORD_CHANGE", "users", u.id, "Senha alterada.");

  s->must_change_password = false;
  return true;
}