#include "security/rbac.h"

uint32_t rbac_permissions_for_role(int role_id) {
  switch (role_id) {
    case ROLE_ADMIN:
      return 0xFFFFFFFFu;
    case ROLE_ATENDENTE:
      return PERM_PATIENT_CRUD | PERM_CHECKIN_CREATE | PERM_REPORT_VIEW;
    case ROLE_ENFERMAGEM:
      return PERM_TRIAGE_WRITE | PERM_REPORT_VIEW;
    case ROLE_MEDICO:
      return PERM_CONSULT_WRITE | PERM_PRESCRIPTION_WRITE | PERM_REPORT_VIEW;
    case ROLE_FARMACIA:
      return PERM_PHARMACY_STOCK | PERM_REPORT_VIEW;
    default:
      return 0;
  }
}

bool rbac_has(uint32_t perms, Permission p) {
  return (perms & (uint32_t)p) != 0u;
}

const char* rbac_role_name(int role_id) {
  switch (role_id) {
    case ROLE_ADMIN: return "ADMIN";
    case ROLE_ATENDENTE: return "ATENDENTE";
    case ROLE_ENFERMAGEM: return "ENFERMAGEM";
    case ROLE_MEDICO: return "MEDICO";
    case ROLE_FARMACIA: return "FARMACIA";
    default: return "DESCONHECIDO";
  }
}