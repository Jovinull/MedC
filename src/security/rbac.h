#ifndef SECURITY_RBAC_H
#define SECURITY_RBAC_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  PERM_USER_MANAGE        = 1u << 0,
  PERM_PATIENT_CRUD       = 1u << 1,
  PERM_CHECKIN_CREATE     = 1u << 2,
  PERM_TRIAGE_WRITE       = 1u << 3,
  PERM_CONSULT_WRITE      = 1u << 4,
  PERM_PRESCRIPTION_WRITE = 1u << 5,
  PERM_PHARMACY_STOCK     = 1u << 6,
  PERM_REPORT_VIEW        = 1u << 7,
  PERM_AUDIT_VIEW         = 1u << 8
} Permission;

typedef enum {
  ROLE_ADMIN = 1,
  ROLE_ATENDENTE = 2,
  ROLE_ENFERMAGEM = 3,
  ROLE_MEDICO = 4,
  ROLE_FARMACIA = 5
} RoleId;

uint32_t rbac_permissions_for_role(int role_id);
bool rbac_has(uint32_t perms, Permission p);

const char* rbac_role_name(int role_id);

#endif