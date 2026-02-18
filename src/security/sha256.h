#ifndef SECURITY_SHA256_H
#define SECURITY_SHA256_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint32_t state[8];
  uint64_t bitlen;
  uint8_t data[64];
  size_t datalen;
} Sha256;

void sha256_init(Sha256 *ctx);
void sha256_update(Sha256 *ctx, const uint8_t *data, size_t len);
void sha256_final(Sha256 *ctx, uint8_t hash[32]);

void sha256_hex(const uint8_t hash[32], char out_hex[65]);

#endif