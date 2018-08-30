#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Кодирование из 8-ми битного потока в 7-битный поток и обратно.
 * Старший бит в семибитном потоке означает конец сообщения.
*/
typedef struct
{
    uint8_t full_byte;
    uint8_t rest_byte;
    uint8_t rest_bit_count;
    //Это последний байт
    bool last;
    //Это полный байт для отправки в поток
    bool full;
} Bit7EncodeContext;

typedef struct
{
    uint16_t data;
    uint8_t bit_count;
    bool last;
} Bit7DecodeContext;


//inline int BIT7_ARRAY_SIZE(int size8) { return ((size8*8+6)/7); }
#define BIT7_ARRAY_SIZE(size8) ((size8*8+6)/7)

void bit7encodeInit(Bit7EncodeContext* data);

//Добавляет полный байт, в поток
//Может добавить два байта подряд
void bit7encodeAddByte(Bit7EncodeContext* data, uint8_t byte);

//Вызывать после bit7encodeAddByte, но перед bit7encodeGetByte
void bit7encodeEndPacket(Bit7EncodeContext* data);

//Есть полный байт, который надо отослать
bool bit7encodeGetByte(Bit7EncodeContext* data, uint8_t* byte);


void bit7decodeInit(Bit7DecodeContext* data);

void bit7decodeAddByte(Bit7DecodeContext* data, uint8_t byte);

//return true если байт прочитался
//два подряд прочитаться не могут
bool bit7decodeGetByte(Bit7DecodeContext* data, uint8_t* byte, bool* complete);

#ifdef __cplusplus
}
#endif
