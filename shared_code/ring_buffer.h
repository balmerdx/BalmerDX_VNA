#pragma once

typedef struct RB_DATA
{
    //Буфер, в котором храняться данные.
    uint8_t* buffer;
    //Количество элементов в буфере.
    uint16_t size;

    //Максимально возможное количество элементов в буфере.
    uint16_t capacity;

    uint16_t read_pos;
    uint16_t write_pos;

} RB_DATA;


void rb_init(RB_DATA* rb, uint8_t* buffer, uint16_t capacity);
bool rb_isEmpty(RB_DATA* rb);
bool rb_isFull(RB_DATA* rb);
bool rb_put(RB_DATA* rb, uint8_t item);
bool rb_get(RB_DATA* rb, uint8_t* item);

bool rb_putArr(RB_DATA* rb, const uint8_t* items, uint16_t size);

bool rb_getArr(RB_DATA* rb, uint8_t* items, uint16_t size);

