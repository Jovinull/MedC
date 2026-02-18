#ifndef SECURITY_AUTH_H
#define SECURITY_AUTH_H

#include <stdbool.h>

#include "app/session.h"

bool auth_login(Session *s, const char *username, const char *password, char err[96]);
bool auth_change_password(Session *s, const char *oldpw, const char *newpw, char err[96]);

#endif