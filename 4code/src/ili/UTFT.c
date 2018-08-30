/*
  ILI9341 2.2 TFT SPI library
  based on UTFT.cpp - Arduino/chipKit library support for Color TFT LCD Boards
  Copyright (C)2010-2013 Henning Karlsen. All right reserved
  2014-2016 - Balmer code
 
*/

#include <string.h>
#include <math.h>

uint32_t* utf_current_font = 0;

#ifdef DISPLAY_ILI9481
#include "hardware/hw_ili9481.h"

enum ENUM_ILI9481
{
    ENTER_SLEEP_MODE = 0x10,
    EXIT_SLEEP_MODE = 0x11,
    ENTER_NORMAL_MODE = 0x13,

    EXIT_INVERT_MODE = 0x20,
    ENTER_INVERT_MODE = 0x21,
    SET_DISPLAY_ON = 0x29,

    SET_COLUMN_ADDRESS = 0x2a,
    SET_PAGE_ADDRESS   = 0x2b,

    WRITE_MEMORY_START = 0x2c,

    SET_SCROLL_AREA = 0x33,
    SET_ADDRESS_MODE = 0x36,
    SET_SCROLL_START = 0x37,
    SET_PIXEL_FORMAT = 0x3a,

    FRAME_MEMORY_ACCESS_AND_INTERFACE_SETTING = 0xb3,

    PANEL_DRIVING = 0xc0,
    DISPLAY_TIMING_SETTING_NORMAL_MODE = 0xc1,
    DISPLAY_TIMING_SETTING_IDLE_MODE = 0xc3,
    FRAME_RATE_AND_INVERSION_CONTROL = 0xc5,
    INTERFACE_CONTROL = 0xc6,

    GAMMA_SETTING = 0xc8,

    POWER_SETTING = 0xd0,
    VCOM = 0xd1,
    POWER_SETTING_NORMAL_MODE = 0xd2,

    // SET_ADDRESS_MODE bits
    VERTICAL_FLIP         = 1,
    HORIZONTAL_FLIP       = 1 << 1,
    BGR                   = 1 << 3,
    VERTICAL_ORDER        = 1 << 4,
    PAGECOL_SELECTION     = 1 << 5,
    COLUMN_ADDRESS_ORDER  = 1 << 6,
    PAGE_ADDRESS_ORDER    = 1 << 7,

    // POWER_SETTING bits

    VC_095=0, VC_090=1, VC_085=2, VC_080=3, VC_075=4, VC_070=5, VC_DISABLE=6, VC_100=7,
    BT_6_5=0, BT_6_4=1, BT_6_3=2, BT_5_5=3, BT_5_4=4, BT_5_3=5, BT_4_4=6, BT_4_3=7,
    VCIRE=1 << 7,

    // INTERFACE_CONTROL bits

    DPL=1, EPL=1 << 1, HSPL=1 << 3, VSPL=1 << 4, SDA_EN=1 << 7,

    // SET_COLOUR_MODE bits

    COLOURS_8=0x11,
    COLOURS_64K=0x55,
    COLOURS_262K=0x66
};

static int disp_x_size=320-1, disp_y_size=480-1;

#else
#define DISPLAY_ILI9341
#include "hardware/hw_ili9341.h"

enum ENUM_ILI9341
{
    ENTER_SLEEP_MODE = 0x10,
    EXIT_SLEEP_MODE = 0x11,
    ENTER_NORMAL_MODE = 0x13,

    EXIT_INVERT_MODE = 0x20,
    ENTER_INVERT_MODE = 0x21,
    SET_DISPLAY_ON = 0x29,

    SET_COLUMN_ADDRESS = 0x2a,
    SET_PAGE_ADDRESS   = 0x2b,

    WRITE_MEMORY_START = 0x2c,

    SET_SCROLL_AREA = 0x33,
    SET_ADDRESS_MODE = 0x36,
    SET_SCROLL_START = 0x37,
    SET_PIXEL_FORMAT = 0x3a,

    // SET_ADDRESS_MODE bits
    BGR                   = 1 << 3,
    VERTICAL_ORDER        = 1 << 4,
    PAGECOL_SELECTION     = 1 << 5,
    COLUMN_ADDRESS_ORDER  = 1 << 6,
    PAGE_ADDRESS_ORDER    = 1 << 7,

    // SET_COLOUR_MODE bits
    COLOURS_64K=0x55,
};

static int disp_x_size=239, disp_y_size=319;

#endif
#include "UTFT.h"
#include "delay.h"

#define TFT_RST_OFF HwLcdPinRst(1)
#define TFT_RST_ON  HwLcdPinRst(0)

#define swap(type, i, j) {type t = i; i = j; j = t;}
#define fontbyte(x) (cfont.font[x])


/*
	The functions and variables below should not normally be used.
	They have been left publicly available for use in add-on libraries
	that might need access to the lower level functions of UTFT.

	Please note that these functions and variables are not documented
	and I do not provide support on how to use them.
*/
static uint16_t front_color;
static uint16_t back_color;

static uint8_t orient;
//uint8_t display_model, display_transfer_mode, display_serial_mode;
//regtype *P_RS, *P_WR, *P_CS, *P_RST, *P_SDA, *P_SCL, *P_ALE;
//regsize B_RS, B_WR, B_CS, B_RST, B_SDA, B_SCL, B_ALE;
static _current_font	cfont;
static bool _transparent;

void UTFT_setPixel(uint16_t color);
void UTFT_drawHLine(int x, int y, int l);
void UTFT_drawVLine(int x, int y, int l);
void UTFT_clrXY();
void UTFT_rotateChar(uint8_t c, int x, int y, int pos, int deg);

static void WRITE_DATA(uint8_t VL);
uint8_t UTFT_readID(void);
uint8_t UTFT_Read_Register(uint8_t Addr, uint8_t xParameter);

void delay(uint32_t nTime)
{
	if(nTime==0)
		nTime = 1;
	DelayMs(nTime);
}

static void sendCMD(uint8_t index)
{
    HwLcdPinDC(0);
    HwLcdPinCE(0);
    HwLcdSend(index);
    HwLcdPinCE(1);
}

static void WRITE_DATA(uint8_t data)
{
    HwLcdPinDC(1);
    HwLcdPinCE(0);
    HwLcdSend(data);
    HwLcdPinCE(1);
}

//Использовать только для записи пикселей в дисплей
static void LCD_Write_DATA16(uint16_t data)
{
    HwLcdPinDC(1);
    HwLcdPinCE(0);
	HwLcdSend16NoWait(data);
	HwLcdWait();
    HwLcdPinCE(1);
}

//Использовать только для записи данных в команду
static void LCD_WriteCommand_DATA16(uint16_t data)
{
#ifdef DISPLAY_SPI
    HwLcdPinDC(1);
    HwLcdPinCE(0);
    HwLcdSend16NoWait(data);
    HwLcdWait();
    HwLcdPinCE(1);
#else
    HwLcdPinDC(1);
    HwLcdPinCE(0);
    HwLcdSend16NoWait(data>>8);
    HwLcdSend16NoWait(data&0xFF);
    HwLcdWait();
    HwLcdPinCE(1);
#endif
}

void TFTBeginData()
{
    HwLcdPinDC(1);
    HwLcdPinCE(0);
}

#define TFTWriteData(data) HwLcdSend16NoWait(data)

void TFTEndData()
{
	HwLcdWait();
    HwLcdPinCE(1);
}


static uint8_t Read_Register(uint8_t Addr, uint8_t xParameter)
{
    uint8_t data=0;
    sendCMD(0xd9);                                                      /* ext command                  */
    WRITE_DATA(0x10+xParameter);                                        /* 0x11 is the first Parameter  */
    HwLcdPinDC(0);
    HwLcdPinCE(0);
    HwLcdSend(Addr);
    HwLcdPinDC(1);
    data = HwLcdSend(0);
    HwLcdPinCE(1);
    return data;
}

uint8_t UTFT_readID(void)
{
    uint8_t i=0;
    uint8_t data[3] ;
    uint8_t ID[3] = {0x00, 0x93, 0x41};
    uint8_t ToF=1;
    for(i=0;i<3;i++)
    {
        data[i]=Read_Register(0xd3,i+1);
        if(data[i] != ID[i])
        {
            ToF=0;
        }
    }
    if(!ToF)                                                            /* data!=ID                     */
    {
    	//error!
    }
    return ToF;
}

void UTFT_InitLCD(uint8_t orientation)
{
    HwLcdInit();
    HwLcdPinLed(1);
    HwLcdPinCE(1);
    HwLcdPinDC(1);

    HwLcdPinRst(0);
    delay(50);
    HwLcdPinRst(1);

    orient=orientation;
    if (orient==UTFT_LANDSCAPE)
    {
        swap(int, disp_x_size, disp_y_size);
    }


/*
    for(uint8_t i=0; i<3; i++)
    {
        uint8_t TFTDriver = UTFT_readID();
    }
*/
    delay(1);

#ifdef DISPLAY_ILI9481
    sendCMD(EXIT_SLEEP_MODE);
    delay(50);
    sendCMD(ENTER_NORMAL_MODE);

    // power setting
    sendCMD(POWER_SETTING);
    WRITE_DATA(7);
    WRITE_DATA((1 << 6) | 3);
    WRITE_DATA(5 | (1 << 4));

    // VCom (more power settings)

    sendCMD(VCOM);
    WRITE_DATA(0); // register D1 for setting VCom
    WRITE_DATA(0);
    WRITE_DATA(0);

    // power setting for normal mode

    sendCMD(POWER_SETTING_NORMAL_MODE);
    WRITE_DATA(1);    // drivers on
    WRITE_DATA(2);    // fosc ratios

    // panel driving setting

    sendCMD(PANEL_DRIVING);
    WRITE_DATA(0);
    WRITE_DATA(0x3b);
    WRITE_DATA(0);
    WRITE_DATA(2);
    WRITE_DATA(1 | (1 << 4));

    // display timing (c1)

    sendCMD(DISPLAY_TIMING_SETTING_NORMAL_MODE);
    WRITE_DATA(1 << 4);         // line inversion, 1:1 internal clock
    WRITE_DATA(16);             // 1 line = 16 clocks
    WRITE_DATA((8 << 4) | 8);

    // display timing idle (c3)

    sendCMD(DISPLAY_TIMING_SETTING_IDLE_MODE);
    WRITE_DATA(1 << 4);
    WRITE_DATA(0x20);
    WRITE_DATA(8);

    // frame rate = 72Hz

    sendCMD(FRAME_RATE_AND_INVERSION_CONTROL);
    WRITE_DATA(3);

    // interface control

    sendCMD(INTERFACE_CONTROL);
    WRITE_DATA(1 | (1 << 1)| (1 << 3) | (1 << 4));

    // frame memory access (set DFM for 2 transfers/1 pixel in 18-bit mode)

    sendCMD(FRAME_MEMORY_ACCESS_AND_INTERFACE_SETTING);
    WRITE_DATA(0);
    WRITE_DATA(0);
    WRITE_DATA(0);
    WRITE_DATA(1);      // DFM

    // set the colour depth and orientation

    sendCMD(SET_PIXEL_FORMAT);
    WRITE_DATA(COLOURS_64K);

    sendCMD(SET_ADDRESS_MODE);
    if (orient==UTFT_LANDSCAPE)
        WRITE_DATA(PAGECOL_SELECTION | HORIZONTAL_FLIP |VERTICAL_FLIP | BGR); //landscape
    else
        WRITE_DATA(HORIZONTAL_FLIP | BGR);    //portrait BGR

    //gamma
    /*
    sendCMD(GAMMA_SETTING);
    uint8_t gamma[12] = {0,0xf3,0,0xbc,0x50,0x1f,0,7,0x7f,0x7,0xf,0};
    for(int i=0; i<12; i++)
        WRITE_DATA(gamma[i]);
    */

    // display on
    delay(100);
    sendCMD(SET_DISPLAY_ON);

    //sendCMD(EXIT_INVERT_MODE);
    sendCMD(ENTER_INVERT_MODE);
#endif

#ifdef DISPLAY_ILI9341
    sendCMD(0xCB); //Power control A
	WRITE_DATA(0x39);
	WRITE_DATA(0x2C);
	WRITE_DATA(0x00);
	WRITE_DATA(0x34);
	WRITE_DATA(0x02);
    
    sendCMD(0xCF); //Power control B
	WRITE_DATA(0x00);
	WRITE_DATA(0XC1);
	WRITE_DATA(0X30);
    
    sendCMD(0xE8); //Driver timing control A
	WRITE_DATA(0x85);
	WRITE_DATA(0x00);
	WRITE_DATA(0x78);
    
    sendCMD(0xEA); //Driver timing control B
	WRITE_DATA(0x00);
	WRITE_DATA(0x00);
    
    sendCMD(0xED); //Power on sequence control
	WRITE_DATA(0x64);
	WRITE_DATA(0x03);
	WRITE_DATA(0X12);
	WRITE_DATA(0X81);
    
    sendCMD(0xF7); //Pump ratio control
	WRITE_DATA(0x20);
    
	sendCMD(0xC0);    	//Power control
	WRITE_DATA(0x23);   	//VRH[5:0]
    
	sendCMD(0xC1);    	//Power control
	WRITE_DATA(0x10);   	//SAP[2:0];BT[3:0]
    
	sendCMD(0xC5);    	//VCM control
	WRITE_DATA(0x3e);   	//Contrast
	WRITE_DATA(0x28);
    
	sendCMD(0xC7);    	//VCM control2
	WRITE_DATA(0x86);  	 //--
    
    sendCMD(SET_ADDRESS_MODE);    	// Memory Access Control
    if (orient==UTFT_LANDSCAPE)
        WRITE_DATA(PAGECOL_SELECTION | COLUMN_ADDRESS_ORDER | PAGE_ADDRESS_ORDER | BGR); //landscape
    else
        WRITE_DATA(0x48);  	//C8	   MH=0 BGR=1 ML=0 MV=0 MX=1 MY=0
    
    sendCMD(SET_PIXEL_FORMAT);      //Pixel Format Set
    WRITE_DATA(0x55);   //DBI = 101 DPI=101
    
    sendCMD(0xB1);      //Frame Rate Control
	WRITE_DATA(0x00);
	WRITE_DATA(0x18);
    
	sendCMD(0xB6);    	// Display Function Control
	WRITE_DATA(0x08);
	WRITE_DATA(0x82);
	WRITE_DATA(0x27);
    
	sendCMD(0xF2);    	// 3Gamma Function Disable
	WRITE_DATA(0x00);
    
	sendCMD(0x26);    	//Gamma curve selected
	WRITE_DATA(0x01);
    
	sendCMD(0xE0);    	//Set Gamma
	WRITE_DATA(0x0F);
	WRITE_DATA(0x31);
	WRITE_DATA(0x2B);
	WRITE_DATA(0x0C);
	WRITE_DATA(0x0E);
	WRITE_DATA(0x08);
	WRITE_DATA(0x4E);
	WRITE_DATA(0xF1);
	WRITE_DATA(0x37);
	WRITE_DATA(0x07);
	WRITE_DATA(0x10);
	WRITE_DATA(0x03);
	WRITE_DATA(0x0E);
	WRITE_DATA(0x09);
	WRITE_DATA(0x00);
    
	sendCMD(0XE1);    	//Set Gamma
	WRITE_DATA(0x00);
	WRITE_DATA(0x0E);
	WRITE_DATA(0x14);
	WRITE_DATA(0x03);
	WRITE_DATA(0x11);
	WRITE_DATA(0x07);
	WRITE_DATA(0x31);
	WRITE_DATA(0xC1);
	WRITE_DATA(0x48);
	WRITE_DATA(0x08);
	WRITE_DATA(0x0F);
	WRITE_DATA(0x0C);
	WRITE_DATA(0x31);
	WRITE_DATA(0x36);
	WRITE_DATA(0x0F);
    
    sendCMD(EXIT_SLEEP_MODE);    	//Exit Sleep
	delay(120); 
    
    sendCMD(SET_DISPLAY_ON);    //Display on
	delay(120);
#endif
    
	cfont.font=0;
	_transparent = false;
}

//Установить прямоугольную область и начать записть в видеопамять
void UTFT_setXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#ifdef DISPLAY_SPI
    sendCMD(SET_COLUMN_ADDRESS);
	TFTBeginData();
	TFTWriteData(x1);
	TFTWriteData(x2);
	TFTEndData();
    sendCMD(SET_PAGE_ADDRESS);
	TFTBeginData();
	TFTWriteData(y1);
	TFTWriteData(y2);
	TFTEndData();
#else
    //Только 8 младших бит используются в команде
    sendCMD(SET_COLUMN_ADDRESS);
    TFTBeginData();
    TFTWriteData(x1>>8);
    TFTWriteData(x1&0xFF);
    TFTWriteData(x2>>8);
    TFTWriteData(x2&0xFF);
    TFTEndData();
    sendCMD(SET_PAGE_ADDRESS);
    TFTBeginData();
    TFTWriteData(y1>>8);
    TFTWriteData(y1&0xFF);
    TFTWriteData(y2>>8);
    TFTWriteData(y2&0xFF);
    TFTEndData();
#endif

    //memory write command
    sendCMD(WRITE_MEMORY_START);
}

void UTFT_clrXY()
{
    UTFT_setXY(0,0,disp_x_size,disp_y_size);
}

void UTFT_drawRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}

    UTFT_drawHLine(x1, y1, x2-x1);
    UTFT_drawHLine(x1, y2, x2-x1);
    UTFT_drawVLine(x1, y1, y2-y1);
    UTFT_drawVLine(x2, y1, y2-y1);
}

void UTFT_drawRoundRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		UTFT_drawPixel(x1+1,y1+1);
		UTFT_drawPixel(x2-1,y1+1);
		UTFT_drawPixel(x1+1,y2-1);
		UTFT_drawPixel(x2-1,y2-1);
		UTFT_drawHLine(x1+2, y1, x2-x1-4);
		UTFT_drawHLine(x1+2, y2, x2-x1-4);
		UTFT_drawVLine(x1, y1+2, y2-y1-4);
		UTFT_drawVLine(x2, y1+2, y2-y1-4);
	}
}

void UTFT_fillRect(int x1, int y1, int x2, int y2)
{
    if (x1>x2)
    {
        swap(int, x1, x2);
    }
    if (y1>y2)
    {
        swap(int, y1, y2);
    }

    UTFT_setXY(x1, y1, x2, y2);

    TFTBeginData();
    int size = (x2-x1+1)*(y2-y1+1);
    for (int i=0; i<size; i++)
    {
        TFTWriteData(front_color);
    }
    TFTEndData();
}

void UTFT_fillRectBack(int x1, int y1, int x2, int y2)
{
    uint16_t old_color = UTFT_getColor();
    UTFT_setColorW(UTFT_getBackColor());
    UTFT_fillRect(x1, y1, x2, y2);
    UTFT_setColorW(old_color);
}


void UTFT_fillRoundRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		swap(int, x1, x2);
	}
	if (y1>y2)
	{
		swap(int, y1, y2);
	}

	if ((x2-x1)>4 && (y2-y1)>4)
	{
		for (int i=0; i<((y2-y1)/2)+1; i++)
		{
			switch(i)
			{
			case 0:
				UTFT_drawHLine(x1+2, y1+i, x2-x1-4);
				UTFT_drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				UTFT_drawHLine(x1+1, y1+i, x2-x1-2);
				UTFT_drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				UTFT_drawHLine(x1, y1+i, x2-x1);
				UTFT_drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}

void UTFT_drawCircle(int x, int y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
 
	UTFT_setXY(x, y + radius, x, y + radius);
	LCD_Write_DATA16(front_color);
	UTFT_setXY(x, y - radius, x, y - radius);
	LCD_Write_DATA16(front_color);
	UTFT_setXY(x + radius, y, x + radius, y);
	LCD_Write_DATA16(front_color);
	UTFT_setXY(x - radius, y, x - radius, y);
	LCD_Write_DATA16(front_color);
 
	while(x1 < y1)
	{
		if(f >= 0) 
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
		UTFT_setXY(x + x1, y + y1, x + x1, y + y1);
		LCD_Write_DATA16(front_color);
		UTFT_setXY(x - x1, y + y1, x - x1, y + y1);
		LCD_Write_DATA16(front_color);
		UTFT_setXY(x + x1, y - y1, x + x1, y - y1);
		LCD_Write_DATA16(front_color);
		UTFT_setXY(x - x1, y - y1, x - x1, y - y1);
		LCD_Write_DATA16(front_color);
		UTFT_setXY(x + y1, y + x1, x + y1, y + x1);
		LCD_Write_DATA16(front_color);
		UTFT_setXY(x - y1, y + x1, x - y1, y + x1);
		LCD_Write_DATA16(front_color);
		UTFT_setXY(x + y1, y - x1, x + y1, y - x1);
		LCD_Write_DATA16(front_color);
		UTFT_setXY(x - y1, y - x1, x - y1, y - x1);
		LCD_Write_DATA16(front_color);
	}
}

void UTFT_fillCircle(int x, int y, int radius)
{
	for(int y1=-radius; y1<=0; y1++) 
		for(int x1=-radius; x1<=0; x1++)
			if(x1*x1+y1*y1 <= radius*radius) 
			{
                UTFT_drawHLine(x+x1, y+y1, 2*(-x1));
                UTFT_drawHLine(x+x1, y-y1, 2*(-x1));
				break;
			}
}

void UTFT_clrScr()
{
	UTFT_fillScrW(back_color);
}

uint16_t UTFT_color(uint8_t r, uint8_t g, uint8_t b)
{
    return UTFT_COLOR(r,g,b);
}

void UTFT_fillScr(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t color = UTFT_color(r,g,b);
	UTFT_fillScrW(color);
}

void UTFT_fillScrW(uint16_t color)
{	
    UTFT_clrXY();
    TFTBeginData();
    for (int i=0; i<((disp_x_size+1)*(disp_y_size+1)); i++)
    {
    	TFTWriteData(color);
    }
    TFTEndData();
}

void UTFT_setColor(uint8_t r, uint8_t g, uint8_t b)
{
    front_color = UTFT_color(r,g,b);
}

void UTFT_setColorW(uint16_t color)
{
    front_color = color;
}

uint16_t UTFT_getColor()
{
    return front_color;
}

void UTFT_setBackColor(uint8_t r, uint8_t g, uint8_t b)
{
	_transparent=false;
    back_color = UTFT_color(r,g,b);
}

void UTFT_setBackColorW(uint32_t color)
{
	if (color==VGA_TRANSPARENT)
		_transparent=true;
	else
	{
        back_color = (uint16_t)color;
		_transparent=false;
	}
}

uint16_t UTFT_getBackColor()
{
    return back_color;
}

void UTFT_setPixel(uint16_t color)
{
	LCD_Write_DATA16(color);	// rrrrrggggggbbbbb
}

void UTFT_drawPixel(int x, int y)
{
	UTFT_setXY(x, y, x, y);
	UTFT_setPixel(front_color);
}

void UTFT_drawLine(int x1, int y1, int x2, int y2)
{
	if (y1==y2)
		UTFT_drawHLine(x1, y1, x2-x1);
	else if (x1==x2)
		UTFT_drawVLine(x1, y1, y2-y1);
	else
	{
		unsigned int	dx = (x2 > x1 ? x2 - x1 : x1 - x2);
		short			xstep =  x2 > x1 ? 1 : -1;
		unsigned int	dy = (y2 > y1 ? y2 - y1 : y1 - y2);
		short			ystep =  y2 > y1 ? 1 : -1;
		int				col = x1, row = y1;

		if (dx < dy)
		{
			int t = - (dy >> 1);
			while (true)
			{
				UTFT_setXY (col, row, col, row);
				LCD_Write_DATA16(front_color);
				if (row == y2)
					break;
				row += ystep;
				t += dx;
				if (t >= 0)
				{
					col += xstep;
					t   -= dy;
				}
			} 
		}
		else
		{
			int t = - (dx >> 1);
			while (true)
			{
				UTFT_setXY (col, row, col, row);
				LCD_Write_DATA16(front_color);
				if (col == x2)
					break;
				col += xstep;
				t += dy;
				if (t >= 0)
				{
					row += ystep;
					t   -= dx;
				}
			} 
		}
	}
}

void UTFT_drawHLine(int x, int y, int l)
{
	if (l<0)
	{
		l = -l;
		x -= l;
	}
	UTFT_setXY(x, y, x+l, y);
	
	TFTBeginData();
	for (int i=0; i<l+1; i++)
	{
		TFTWriteData(front_color);
	}
	TFTEndData();
}

void UTFT_drawVLine(int x, int y, int l)
{
	if (l<0)
	{
		l = -l;
		y -= l;
	}

	UTFT_setXY(x, y, x, y+l);

	TFTBeginData();
	for (int i=0; i<l+1; i++)
	{
		TFTWriteData(front_color);
	}
	TFTEndData();
}

void UTFT_printChar(uint8_t c, int x, int y)
{
	if (!_transparent)
	{
        UTFT_setXY(x,y,x+cfont.x_size-1,y+cfont.y_size-1);

        TFTBeginData();
        uint16_t temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
        int size = ((cfont.x_size/8)*cfont.y_size);
        uint8_t* data = &fontbyte(temp);
        uint8_t* data_end = data + size;
        while(data < data_end)
        {
            uint8_t ch= *data++;
            /*
            for(int i=0;i<8;i++)
            {
                TFTWriteData((ch&(1<<(7-i)))?front_color:back_color);
            }
            */
            for(int i=7;i>=0;i--)
            {
                TFTWriteData((ch&(1<<i))?front_color:back_color);
            }
        }
        TFTEndData();
	}
	else
	{
        uint16_t temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
        for(int j=0;j<cfont.y_size;j++)
		{
			for (int zz=0; zz<(cfont.x_size/8); zz++)
			{
                uint8_t ch=fontbyte(temp+zz);
                for(int i=0;i<8;i++)
				{   
					UTFT_setXY(x+i+(zz*8),y+j,x+i+(zz*8)+1,y+j+1);
				
					if((ch&(1<<(7-i)))!=0)   
					{
                        UTFT_setPixel(front_color);
					} 
				}
			}
			temp+=(cfont.x_size/8);
		}
	}
}

void UTFT_rotateChar(uint8_t c, int x, int y, int pos, int deg)
{
    uint8_t i,j,ch;
    uint16_t temp;
	int newx,newy;
	double radian;
	radian=deg*0.0175;  


	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++) 
	{
		for (int zz=0; zz<(cfont.x_size/8); zz++)
		{
			ch=fontbyte(temp+zz);
			for(i=0;i<8;i++)
			{   
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

				UTFT_setXY(newx,newy,newx+1,newy+1);
				
				if((ch&(1<<(7-i)))!=0)   
				{
                    UTFT_setPixel(front_color);
				} 
				else  
				{
					if (!_transparent)
                        UTFT_setPixel(back_color);
				}   
			}
		}
		temp+=(cfont.x_size/8);
	}
}

void UTFT_print(const char *st, int x, int y)
{
    UTFT_printRotate(st, x, y, 0);
}

void UTFT_printRotate(const char *st, int x, int y, int deg)
{
	int stl, i;

	stl = strlen(st);

	if (x==UTFT_RIGHT)
		x=(disp_x_size+1)-(stl*cfont.x_size);
	if (x==UTFT_CENTER)
		x=((disp_x_size+1)-(stl*cfont.x_size))/2;

	for (i=0; i<stl; i++)
    {
        char c = *st++;
        if(c==NUM_SPACE)
            c = 32;

		if (deg==0)
            UTFT_printChar(c, x + (i*(cfont.x_size)), y);
		else
            UTFT_rotateChar(c, x, y, i, deg);
    }
}


void UTFT_printNumI(long num, int x, int y, int length, char filler)
{
	char st[27];
    intToString(st, num, length, filler);
    UTFT_print(st, x, y);
}

void UTFT_printNumF(float value, int x, int y, int places, int minwidth, bool rightjustify)
{
    char st[27];
    floatToString(st, 27, value, places, minwidth, rightjustify);
    UTFT_print(st, x, y);
}

void UTFT_setFont(const uint8_t* font)
{
    cfont.font=(uint8_t*)font;
	cfont.x_size=fontbyte(0);
	cfont.y_size=fontbyte(1);
	cfont.offset=fontbyte(2);
	cfont.numchars=fontbyte(3);

    utf_current_font = 0;
}

uint8_t* UTFT_getFont()
{
	return cfont.font;
}

uint8_t UTFT_getFontXsize()
{
	return cfont.x_size;
}

uint8_t UTFT_getFontYsize()
{
	return cfont.y_size;
}

void UTFT_drawBitmap(int x, int y, const Bitmap16bit* bitmap)
{
    int sx = bitmap->width;
    int sy = bitmap->height;
    const uint16_t* data = bitmap->colors;

    UTFT_setXY(x, y, x+sx-1, y+sy-1);
    TFTBeginData();
    int sxy = sx*sy;
    for (int tc=0; tc<sxy; tc++)
    {
        uint16_t col=data[tc];
        TFTWriteData(col);
    }
    TFTEndData();
}

void UTFT_drawBitmapS(int x, int y, const Bitmap16bit* bitmap, int scale)
{
    if (scale==1)
    {
        UTFT_drawBitmap(x, y, bitmap);
        return;
    }

    int sx = bitmap->width;
    int sy = bitmap->height;
    const uint16_t* data = bitmap->colors;

    int tx, ty, tsx, tsy;

    for (ty=0; ty<sy; ty++)
    {
        UTFT_setXY(x, y+(ty*scale), x+((sx*scale)-1), y+(ty*scale)+scale);
        TFTBeginData();
        for (tsy=0; tsy<scale; tsy++)
            for (tx=0; tx<sx; tx++)
            {
                uint16_t col=data[(ty*sx)+tx];
                for (tsx=0; tsx<scale; tsx++)
                    TFTWriteData(col);
            }
        TFTEndData();
    }
}

void UTFT_drawBitmapR(int x, int y, const Bitmap16bit* bitmap, int deg, int rox, int roy)
{
	if (deg==0)
        UTFT_drawBitmap(x, y, bitmap);
	else
	{
        int sx = bitmap->width;
        int sy = bitmap->height;
        const uint16_t* data = bitmap->colors;

        float radian = deg*0.01745f;
		float cr = cosf(radian);
		float sr = sinf(radian);
        for (int ty=0; ty<sy; ty++)
        for (int tx=0; tx<sx; tx++)
		{
            uint16_t col=data[(ty*sx)+tx];

            int newx=x+rox+(((tx-rox)*cr)-((ty-roy)*sr));
            int newy=y+roy+(((ty-roy)*cr)+((tx-rox)*sr));

			UTFT_setXY(newx, newy, newx, newy);
			LCD_Write_DATA16(col);
		}
    }
}

void UTFT_lcdOff()
{
    HwLcdPinLed(0);
}

void UTFT_lcdOn()
{
    HwLcdPinLed(1);
}

void UTFT_setContrast(char c)
{

}

int UTFT_getDisplayXSize()
{
    return disp_x_size+1;
}

int UTFT_getDisplayYSize()
{
    return disp_y_size+1;
}

void UTFT_verticalScrollDefinition(uint16_t topFixedArea, uint16_t verticalScrollArea, uint16_t bottomFixedArea)
{
    sendCMD(SET_SCROLL_AREA); //VSCRDEF (Vertical Scrolling Definition)
    LCD_WriteCommand_DATA16(topFixedArea);
    LCD_WriteCommand_DATA16(verticalScrollArea);
    LCD_WriteCommand_DATA16(bottomFixedArea);
}

void UTFT_verticalScroll(uint16_t vsp)
{
    sendCMD(SET_SCROLL_START);
    LCD_WriteCommand_DATA16(vsp);
}

uint16_t UTFT_getColor4(int x, int y, const Bitmap4bit* bitmap)
{
    int offset = (bitmap->width+1)/2*y+x/2;
    uint8_t data = bitmap->colors[offset];
    if(x&1)
        data = data>>4;
    else
        data = data&0xf;

    return bitmap->palette[data];
}

void UTFT_drawBitmap4(int x, int y, const Bitmap4bit* bitmap)
{
    int sx = bitmap->width;
    int sy = bitmap->height;

    UTFT_setXY(x, y, x+sx-1, y+sy-1);
    TFTBeginData();
    for (int ty=0; ty<sy; ty++)
    for (int tx=0; tx<sx; tx++)
    {
        uint16_t col=UTFT_getColor4(tx, ty, bitmap);
        TFTWriteData(col);
    }
    TFTEndData();
}

void UTFT_drawUtfChar1bit(int x, int y, int char_width, int char_height, uint32_t* data, int top_space, int bottom_space)
{
    UTFT_setXY(x, y, x+char_width-1, y+char_height+top_space+bottom_space-1);
    TFTBeginData();
    int sz_top = top_space*char_width;
    for(int i=0; i<sz_top; i++)
        TFTWriteData(back_color);

    int pixels = char_width*char_height;
    int size32 = pixels/32;
    int reminder32 = pixels%32;
    for(int sz=0; sz<size32; sz++)
    {
        uint32_t d = *data++;
        for(int i=0; i<32; i++)
            TFTWriteData((d&(1u<<i))?front_color:back_color);
    }

    uint32_t d = *data++;
    for(int i=0; i<reminder32; i++)
        TFTWriteData((d&(1u<<i))?front_color:back_color);

    int sz_bottom = bottom_space*char_width;
    for(int i=0; i<sz_bottom; i++)
        TFTWriteData(back_color);

    TFTEndData();
}


void UTFT_drawUtfChar2bit(int x, int y, int char_width, int char_height, uint32_t* data, int top_space, int bottom_space, uint16_t palette[4])
{
    UTFT_setXY(x, y, x+char_width-1, y+char_height+top_space+bottom_space-1);
    TFTBeginData();
    int sz_top = top_space*char_width;
    for(int i=0; i<sz_top; i++)
        TFTWriteData(back_color);

    int pixels = char_width*char_height;
    int size32 = pixels/16;
    int reminder32 = (pixels%16)*2;
    for(int sz=0; sz<size32; sz++)
    {
        uint32_t d = *data++;
        for(int i=0; i<32; i+=2)
        {
            TFTWriteData(palette[(d>>i)&3]);
        }
    }

    uint32_t d = *data++;
    for(int i=0; i<reminder32; i+=2)
        TFTWriteData(palette[(d>>i)&3]);

    int sz_bottom = bottom_space*char_width;
    for(int i=0; i<sz_bottom; i++)
        TFTWriteData(back_color);

    TFTEndData();
}
