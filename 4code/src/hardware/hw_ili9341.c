#include <stdint.h>

#ifndef DISPLAY_ILI9481

#include "hw_ili9341.h"

#include "delay.h"

#define TFT_SPI			SPI2
#define TFT_AF_SPI GPIO_AF_SPI2
#define TFT_PORT_SPI	GPIOB
#define PIN_SOURCE_SCK	GPIO_PinSource13
#define PIN_SOURCE_MISO	GPIO_PinSource14
#define PIN_SOURCE_MOSI	GPIO_PinSource15
#define PIN_SCK		(1<<PIN_SOURCE_SCK)
#define PIN_MISO	(1<<PIN_SOURCE_MISO)
#define PIN_MOSI	(1<<PIN_SOURCE_MOSI)

#define TFT_PORT_LED	GPIOC
#define PIN_LED		GPIO_Pin_8

#define TFT_PORT_RST	GPIOC
#define PIN_RST		GPIO_Pin_6

#define TFT_PORT_DC	GPIOC
#define PIN_DC		GPIO_Pin_7

#define TFT_PORT_CS	GPIOB
#define PIN_CS		GPIO_Pin_12



void HwLcdInit()
{
	GPIO_InitTypeDef gpio;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);

	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

	gpio.GPIO_Pin = PIN_LED;
    GPIO_Init(TFT_PORT_LED, &gpio);

	gpio.GPIO_Pin = PIN_DC;
    GPIO_Init(TFT_PORT_DC, &gpio);

	gpio.GPIO_Pin = PIN_RST;
    GPIO_Init(TFT_PORT_RST, &gpio);

	gpio.GPIO_Pin = PIN_CS;
    GPIO_Init(TFT_PORT_CS, &gpio);


	gpio.GPIO_Pin = PIN_SCK | PIN_MOSI | PIN_MISO;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TFT_PORT_SPI, &gpio);

	// connect SPI pins to SPI alternate function
	GPIO_PinAFConfig(TFT_PORT_SPI, PIN_SOURCE_SCK, TFT_AF_SPI);
	GPIO_PinAFConfig(TFT_PORT_SPI, PIN_SOURCE_MISO, TFT_AF_SPI);
	GPIO_PinAFConfig(TFT_PORT_SPI, PIN_SOURCE_MOSI, TFT_AF_SPI);

    //SPI_StructInit(&spi);
	SPI_InitTypeDef spi;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(TFT_SPI, &spi);
	SPI_Cmd(TFT_SPI, ENABLE);
	SPI_NSSInternalSoftwareConfig(TFT_SPI, SPI_NSSInternalSoft_Set);
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
	GPIO_WriteBit(TFT_PORT_LED, PIN_LED, on);
}

uint8_t HwLcdSend(uint8_t data)
{
    TFT_SPI->DR = data;
	while( !(TFT_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	while( TFT_SPI->SR & SPI_I2S_FLAG_BSY ); // wait until SPI is not busy anymore
    return TFT_SPI->DR;
}

void HwLcdSend16NoWait(uint16_t data)
{
	TFT_SPI->DR = (data>>8);
	while( !(TFT_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
	TFT_SPI->DR = data&0xFF;
	while( !(TFT_SPI->SR & SPI_I2S_FLAG_TXE) ); // wait until transmit complete
}

void HwLcdWait()
{
	while( TFT_SPI->SR & SPI_I2S_FLAG_BSY );
}

#endif
