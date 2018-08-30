#pragma once

//LCD Hardware routines
//#define LCD_DISCOVERY


void HwLcdInit();
//Подождать несколько милисекунд
void DelaySome();

//Управление пинами 0 - low, 1 - high
void HwLcdPinCE(uint8_t on);
void HwLcdPinDC(uint8_t on);
void HwLcdPinRst(uint8_t on);
void HwLcdPinLed(uint8_t on);
uint8_t HwLcdSend(uint8_t data); //send spi data

//void HwLcdSend16NoWait(uint16_t data);
void HwLcdWait();//Wait send complete

#define TFT_DATA_PORT GPIOD

#ifdef LCD_DISCOVERY
#define TFT_PORT_WR  GPIOC
#define PIN_WR    GPIO_Pin_8
#else
#define TFT_PORT_WR	GPIOB
#define PIN_WR		GPIO_Pin_3
#endif

void HwLcdSend16NoWaitAsm(uint16_t data);
//#define HwLcdSend16NoWait(x) HwLcdSend16NoWaitAsm(x)

__attribute__((always_inline))
inline void HwLcdSend16NoWait(uint16_t data)
{
    TFT_DATA_PORT->ODR = data;
    TFT_PORT_WR->BSRRH = PIN_WR;
    __asm__ volatile("mov r0, r0");
    __asm__ volatile("mov r0, r0");
    __asm__ volatile("mov r0, r0");
    __asm__ volatile("mov r0, r0");
    __asm__ volatile("mov r0, r0");
    __asm__ volatile("mov r0, r0");
    __asm__ volatile("mov r0, r0");
    __asm__ volatile("mov r0, r0");
    __asm__ volatile("mov r0, r0");
    TFT_PORT_WR->BSRRL = PIN_WR;
}


#define  HwLcdWait() (void)0
