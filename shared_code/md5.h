#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct {
   uint8_t data[64];
   uint32_t datalen;
   uint32_t bitlen[2];
   uint32_t state[4];
} MD5_CTX;

#define MD5_HASH_SIZE 16

void md5_init(MD5_CTX *ctx);
void md5_update(MD5_CTX *ctx, uint8_t data[], uint32_t len);
void md5_final(MD5_CTX *ctx, uint8_t hash[]);

#ifdef __cplusplus
}
#endif
