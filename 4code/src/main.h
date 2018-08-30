//balmer@inbox.ru
//STM32F405+CS4272 DSP module
  
#pragma once

#include <stdio.h>
#include <math.h>

#include "stm32f4xx_conf.h"
#include "ili/UTFT.h"
#include "ili/DefaultFonts.h"

#include "delay.h"
#include "dac.h"
#include "quadrature_encoder.h"
#include "job.h"
#include "data.h"
#include "ad9958_drv.h"
#include "interface/plot.h"

#ifdef DISPLAY_ILI9481
#include "usart/usart.h"

__attribute__((always_inline)) inline void DataAdd(uint8_t* data, uint32_t size){ UsartAdd(data, size); }
__attribute__((always_inline)) inline void DataAdd8(uint8_t data){ UsartAdd8(data); }
__attribute__((always_inline)) inline void DataAdd16(uint16_t data){ UsartAdd16(data); }
__attribute__((always_inline)) inline void DataAdd32(uint32_t data){ UsartAdd32(data); }
__attribute__((always_inline)) inline void DataAdd32i(int32_t data){ UsartAdd32i(data); }
__attribute__((always_inline)) inline void DataAdd64(uint64_t data){ UsartAdd64(data); }
__attribute__((always_inline)) inline void DataAddF(float data){ UsartAddF(data); }
__attribute__((always_inline)) inline void DataAddC(complexf data){ UsartAddF(crealf(data)); UsartAddF(cimagf(data));}

__attribute__((always_inline)) inline void DataSend()
{
    UsartSend();
}

#else
/*
#include "usb/data_process.h"

void USBAdd(uint8_t* data, uint32_t size);
void USBAddStr(char* data);
void USBAdd8(uint8_t data);
void USBAdd16(uint16_t data);
void USBAdd32(uint32_t data);
void USBAdd32i(int32_t data);
void USBAddF(float data);
//send command
void USBSend(void);
*/
#endif


void setTX(bool tx);
bool isTX();

extern bool g_jump_to_bootloader;

typedef enum SpiFlashSectors
{
    SPI_FLASH_SECTOR_IF = FLASH_Sector_2, //IF - intermediate frequency 1000 hz
    SPI_FLASH_SECTOR_SOLT0 = FLASH_Sector_8,
    SPI_FLASH_SECTOR_SOLT1 = FLASH_Sector_9,
    SPI_FLASH_SECTOR_SOLT2 = FLASH_Sector_10,
} SpiFlashSectors;

#define FLASH_SECTOR_USER_SETTINGS FLASH_Sector_11

float GetIntermediateFrequency();
