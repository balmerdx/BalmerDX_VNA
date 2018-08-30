/*
 ILI9341 2.2 TFT SPI library
 based on UTFT.cpp - Arduino/chipKit library support for Color TFT LCD Boards
 Copyright (C)2010-2013 Henning Karlsen. All right reserved
 
 Compatible with other UTFT libraries.
 
 Original library you can find at http://electronics.henningkarlsen.com/
  
 This library is free software; you can redistribute it and/or
 modify it under the terms of the CC BY-NC-SA 3.0 license.
 Please see the included documents for further information.
 */

#ifndef UTFT_h
#define UTFT_h

#include "float_to_string.h"

#define UTFT_LEFT 0
#define UTFT_RIGHT 9999
#define UTFT_CENTER 9998

#define UTFT_PORTRAIT 0
#define UTFT_LANDSCAPE 1

//*********************************
// COLORS
//*********************************
// VGA color palette
#define VGA_BLACK		0x0000
#define VGA_WHITE		0xFFFF
#define VGA_RED			0xF800
#define VGA_GREEN		0x07E0
#define VGA_BLUE		0x001F
#define VGA_SILVER		0xC618
#define VGA_GRAY		0x8410
#define VGA_MAROON		0x8000
#define VGA_YELLOW		0xFFE0
#define VGA_OLIVE		0x8400
#define VGA_LIME		0x07E0
#define VGA_AQUA		0x07FF
#define VGA_TEAL		0x0410
#define VGA_NAVY		0x0010
#define VGA_FUCHSIA		0xF81F
#define VGA_PURPLE		0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF

#define UTFT_COLOR(r, g, b) ((((uint16_t)r)&248)<<8 | (((uint16_t)g)&252)<<3 | (((uint16_t)b)&248)>>3)
#define UTFT_RED(color) (((uint16_t)color>>8)&248)
#define UTFT_GREEN(color) (((uint16_t)color>>3)&252)
#define UTFT_BLUE(color) (((uint16_t)color<<3)&248)


#define bitmapdatatype uint16_t*

typedef struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
} _current_font;

typedef struct Bitmap16bit
{
    uint16_t width;
    uint16_t height;
    uint16_t colors[];
} Bitmap16bit;

typedef struct Bitmap4bit
{
    uint16_t width;
    uint16_t height;
    uint16_t palette[16];
    uint8_t colors[];
} Bitmap4bit;

//LANDSCAPE default
void UTFT_InitLCD(uint8_t orientation);
void UTFT_clrScr();
void UTFT_drawPixel(int x, int y);
void UTFT_drawLine(int x1, int y1, int x2, int y2);
void UTFT_fillScr(uint8_t r, uint8_t g, uint8_t b);
void UTFT_fillScrW(uint16_t color);
void UTFT_drawRect(int x1, int y1, int x2, int y2);
void UTFT_drawRoundRect(int x1, int y1, int x2, int y2);
void UTFT_fillRect(int x1, int y1, int x2, int y2);    //use foreground color
void UTFT_fillRectBack(int x1, int y1, int x2, int y2);//use background color
void UTFT_fillRoundRect(int x1, int y1, int x2, int y2);
void UTFT_drawCircle(int x, int y, int radius);
void UTFT_fillCircle(int x, int y, int radius);

void UTFT_setColor(uint8_t r, uint8_t g, uint8_t b);
void UTFT_setColorW(uint16_t color);
uint16_t UTFT_getColor();
void UTFT_setBackColor(uint8_t r, uint8_t g, uint8_t b);
void UTFT_setBackColorW(uint32_t color);
uint16_t UTFT_getBackColor();

//deg=0
void UTFT_print(const char *st, int x, int y);
void UTFT_printRotate(const char *st, int x, int y, int deg);

//length=0, char filler=' '
void UTFT_printNumI(long num, int x, int y, int length, char filler);
void UTFT_printNumF(float value, int x, int y, int places, int minwidth, bool rightjustify);

void UTFT_setFont(const uint8_t* font);
uint8_t* UTFT_getFont();
uint8_t UTFT_getFontXsize();
uint8_t UTFT_getFontYsize();

void UTFT_drawBitmap(int x, int y, const Bitmap16bit* bitmap);

//scaled image
void UTFT_drawBitmapS(int x, int y, const Bitmap16bit* bitmap, int scale);

//rotated image
void UTFT_drawBitmapR(int x, int y, const Bitmap16bit* bitmap, int deg, int rox, int roy);

void UTFT_drawBitmap4(int x, int y, const Bitmap4bit* bitmap);

void UTFT_lcdOff();
void UTFT_lcdOn();
void UTFT_setContrast(char c);
int  UTFT_getDisplayXSize();
int	 UTFT_getDisplayYSize();

//topFixedArea + verticalScrollArea + bottomFixedArea == 320
//default (0,320,0)
void UTFT_verticalScrollDefinition(uint16_t topFixedArea, uint16_t verticalScrollArea, uint16_t bottomFixedArea);
void UTFT_verticalScroll(uint16_t vsp);

//R,G,B to hi color
uint16_t UTFT_color(uint8_t r, uint8_t g, uint8_t b);

uint8_t UTFT_readID();

#endif
