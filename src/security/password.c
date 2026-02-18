#include "security/password.h"
#include "security/sha256.h"

#include <string.h>
#include <time.h>

/* MVP: salt pseudo-aleatório baseado em time + padrões; suficiente p/ offline/local.
   Se quiser robustez criptográfica, pluga /dev/urandom futuramente atrás desta função. */
void password_make_salt(uint8_t salt[16]) {
  uint64_t t = (uint64_t)time(NULL);
  for (int i = 0; i < 16; i++) {
    salt[i] = (uint8_t)((t >> (i % 8) * 8) ^ (uint64_t)(0xA5 + i * 17));
  }
}

void password_hash(const char *password, const uint8_t salt[16], char out_hex[65]) {
  Sha256 ctx;
  sha256_init(&ctx);
  sha256_update(&ctx, salt, 16);
  sha256_update(&ctx, (const uint8_t *)password, strlen(password));
  uint8_t hash[32];
  sha256_final(&ctx, hash);
  sha256_hex(hash, out_hex);
}

bool password_verify(const char *password, const uint8_t salt[16], const char hash_hex[65]) {
  char calc[65];
  password_hash(password, salt, calc);
  return (strncmp(calc, hash_hex, 65) == 0);
}