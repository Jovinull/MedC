#ifndef SECURITY_PASSWORD_H
#define SECURITY_PASSWORD_H

#include <stdbool.h>
#include <stdint.h>

void password_make_salt(uint8_t salt[16]);
void password_hash(const char *password, const uint8_t salt[16], char out_hex[65]);
bool password_verify(const char *password, const uint8_t salt[16], const char hash_hex[65]);

#endif