#pragma once

bool cs4272_Init();

bool cs4272_i2c_write_reg(uint8_t reg, uint8_t data);
uint8_t cs4272_i2c_read_reg(uint8_t reg);

/*
//При инициализации запускается сразу, баги какието
//если запускать после
void cs4272_start();
*/
void cs4272_stop();

#define g_i2s_dma true

#define SOUND_BUFFER_SIZE 2048
extern uint16_t sound_buffer[SOUND_BUFFER_SIZE];

uint16_t cs4272_getPos();
