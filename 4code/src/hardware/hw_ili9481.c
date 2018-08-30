
#ifdef DISPLAY_ILI9481
#include "hw_ili9481.h"

#include "delay.h"

#ifdef LCD_DISCOVERY
#define TFT_PORT_RST  GPIOC
#define PIN_RST    GPIO_Pin_6

#define TFT_PORT_DC  GPIOC
#define PIN_DC    GPIO_Pin_9

#define TFT_PORT_CS  GPIOC
#define PIN_CS    GPIO_Pin_7

#else
#define TFT_PORT_RST	GPIOB
#define PIN_RST		GPIO_Pin_5

#define TFT_PORT_DC	GPIOB
#define PIN_DC		GPIO_Pin_4

#define TFT_PORT_CS	GPIOB
#define PIN_CS		GPIO_Pin_6
#endif

void HwLcdInit()
{
    GPIO_InitTypeDef gpio;

#ifdef LCD_DISCOVERY
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);
#else
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE);
#endif

    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

    gpio.GPIO_Pin = PIN_WR;
    GPIO_Init(TFT_PORT_WR, &gpio);

    gpio.GPIO_Pin = PIN_DC;
    GPIO_Init(TFT_PORT_DC, &gpio);

    gpio.GPIO_Pin = PIN_RST;
    GPIO_Init(TFT_PORT_RST, &gpio);

    gpio.GPIO_Pin = PIN_CS;
    GPIO_Init(TFT_PORT_CS, &gpio);

    //all 16 bit data
    gpio.GPIO_Pin = 0xFFFF;
    GPIO_Init(TFT_DATA_PORT, &gpio);

    GPIO_WriteBit(TFT_PORT_WR, PIN_WR, 1);
}

void DelaySome()
{
    DelayUs(5000);
}

void HwLcdPinCE(uint8_t on)
{
    GPIO_WriteBit(TFT_PORT_CS, PIN_CS, on);
}

void HwLcdPinDC(uint8_t on)
{
    GPIO_WriteBit(TFT_PORT_DC, PIN_DC, on);
}

void HwLcdPinRst(uint8_t on)
{
    GPIO_WriteBit(TFT_PORT_RST, PIN_RST, on);
}

void HwLcdPinLed(uint8_t on)
{

}

uint8_t HwLcdSend(uint8_t data)
{
    HwLcdSend16NoWait(data);
    return 0;
}

#endif
