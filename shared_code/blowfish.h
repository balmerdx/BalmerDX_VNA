#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct {
   uint32_t p[18];
   uint32_t s[4][256];
} BLOWFISH_KEY;

//encode/decode 8 bytes
void blowfish_encrypt(uint8_t in[], uint8_t out[], BLOWFISH_KEY *keystruct);
void blowfish_decrypt(uint8_t in[], uint8_t out[], BLOWFISH_KEY *keystruct);

//max password len 56 bytes
void key_schedule(uint8_t user_key[], BLOWFISH_KEY *keystruct, int len);

#ifdef __cplusplus
}
#endif
