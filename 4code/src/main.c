#include "main.h"
#include "delay.h"
#include "cs4272.h"
#include "process_sound.h"
//#include "hardware/m25p16.h"
//#include "hardware/store_to_spi_flash.h"
#include "interface/font_condensed30.h"
#include "ili/utf_font.h"
#include "string.h"
#include "jump_to_app.h"
#include "bootloader_version.h"
#include "store_user_settings.h"

#ifdef DISPLAY_ILI9481

#else
#include "usb/usbd_cdc_core.h"
#include "usb/usbd_usr.h"
#include "usb/usbd_desc.h"
#include "usb/usbd_cdc_vcp.h"
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
#endif

#include "interface/interface.h"
#include "interface/scene_if_calibration.h"

#include "stm32f4xx_it.h"

void TestSpiFlash();
void TestStoreToSpi();

static bool g_is_tx = false;
bool g_jump_to_bootloader = false;

RCC_ClocksTypeDef RCC_Clocks;

#ifdef DISPLAY_ILI9481
void initTxGpio()
{
    //PC4 - переключение сигнала на RF_INPUT 0-прямой(TX), 1 - отраженный(RX)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef gpio;

    gpio.GPIO_Pin  = GPIO_Pin_4;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_25MHz;
    gpio.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOC, &gpio);

    setTX(false);
}

void setTX(bool tx)
{
    g_is_tx = tx;
    GPIO_WriteBit(GPIOC, GPIO_Pin_4, tx?0:1);
}
#else
void initTxGpio()
{
    //GPIO_Pin_0 - переключение входов CS4242
    //GPIO_Pin_1 - переключение сигнала на RF_INPUT 0-прямой(TX), 1 - отраженный(RX)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin  = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_25MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOC, &gpio);

    gpio.GPIO_Pin  = GPIO_Pin_1;
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_25MHz;
    gpio.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOC, &gpio);

    setTX(false);
}

void setTX(bool tx)
{
    g_is_tx = tx;
    GPIO_WriteBit(GPIOC, GPIO_Pin_1, tx?0:1);
    GPIO_WriteBit(GPIOC, GPIO_Pin_0, tx?0:1);
}
#endif

bool isTX()
{
    return g_is_tx;
}

void DefaultQuant()
{
    DelayUs(10);
    SoundQuant();
    JobQuant();
    UsartQuant();
}

int main(void)
{  
    // SysTick end of count event each 1ms
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

    DelayInit();

    g_default_font = font_condensed30;

#ifdef DISPLAY_ILI9481
    UsartSkipIfInited(true);
    UsartInit();
    //m25p16_init();
#else
    DataReceiveInit();
    USBD_Init(&USB_OTG_dev,
                USB_OTG_FS_CORE_ID,
                &USR_desc,
                &USBD_CDC_cb,
                &USR_cb);
#endif

    UTFT_InitLCD(UTFT_LANDSCAPE);
    //TestStoreToSpi();
    //TestSpiFlash();

    ReadIntermediateFrequencyFromFlash();
    LoadUserSettings();
    ReadCalibration(g_calibration_index);

    UTFT_setFont(FONT8x15);
    UTFT_setColorW(VGA_WHITE);
    UTFT_print("START INIT CS4272", 80, 0);

    bool okAdc = cs4272_Init();
    (void)okAdc;
/*
    UTFT_setColor(0, 255, 255);
    UTFT_print(okAdc?"okAdc=1":"okAdc=0", 0, 16);

    bool b = false;
    while(1)
    {
        DelayMs(500);

        UTFT_printNumI(cs4272_getPos(), 80, 16, 6, ' ');
        UTFT_print(b?"1":"0", 80, 0);
        b = !b;
    }
*/
    AD9958_Init();
    DelayUs(30);
    
    uint32_t freq = 127000;
    uint16_t level = 511;
    AD9958_Set_Frequency(0, freq);
    AD9958_Set_Level(0, level);
    AD9958_Set_Frequency(1, freq+1000);
    AD9958_Set_Level(1, level);

    initTxGpio();

    QuadEncInit();

    RawParamSetStandartFreq();

    InterfaceStart();

    while(1)
    {
        DefaultQuant();
        InterfaceQuant();

        if(g_jump_to_bootloader)
        {
            g_jump_to_bootloader = false;
            UsartWaitSendComplete();
            JumpToApp(BOOTLOADER_FLASH_START);
        }
    }

}

void DeinitPerifery()
{
    //disable CS4272
    cs4272_stop();

    //clock
    TIM_Cmd(TIM2, DISABLE);
    TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM6, DISABLE);
    TIM_Cmd(TIM7, DISABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, DISABLE);
/*
    I2C_Cmd(I2C1, DISABLE);
    I2C_Cmd(I2C2, DISABLE);
    DMA_Cmd(DMA1_Stream0, DISABLE);
    DMA_Cmd(DMA1_Stream3, DISABLE);
    I2S_Cmd(SPI2, DISABLE);
    I2S_Cmd(SPI3, DISABLE);
*/
    //USART
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
    //USART_DeInit(USART1);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);

    //GPIO
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, DISABLE);
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*
void TestSpiFlash()
{
    int x = 0, y = 0;
    UTFT_clrScr();
    UTF_SetFont(font_condensed30);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);

    m25p16_init();
    x = UTF_DrawString(x,y, "TestSpiFlash init");
    x = 0; y+= UTF_Height();

    m25p16_power_up_flash_ram();
    //x = UTF_DrawString(x,y, "power_up_flash_ram"); x = 0; y+= UTF_Height();

    int num_width = UTF_StringWidth("0000");

    if(0)
    {
        uint8_t mem[3];
        m25p16_read_ram_id(mem);
        x = UTF_printNumI(mem[0], x, y, num_width, UTF_RIGHT);
        x = UTF_printNumI(mem[1], x, y, num_width, UTF_RIGHT);
        x = UTF_printNumI(mem[2], x, y, num_width, UTF_RIGHT);
        x = 0; y+= UTF_Height();
    }

    bool id_ok = m25p16_read_ram_id_and_check();
    x = UTF_DrawString(x,y, id_ok?"id OK":"id FAIL");
    x = 0; y+= UTF_Height();
    while(!id_ok);

    uint8_t flash_sector = 0;
    m25p16_ram_sector_erase(flash_sector);
    x = UTF_DrawString(x,y, "ERASE COMPLETE");
    x = 0; y+= UTF_Height();

    if(0)
    {
        uint8_t readed_data;
        m25p16_read_write_flash_ram(1, 1, 0, 0, 0, &readed_data);
        x = UTF_printNumI(readed_data, x, y, num_width, UTF_RIGHT);

        uint8_t write_data = 0x87;
        m25p16_read_write_flash_ram(0, 1, 0, 0, 0, &write_data);
        x = UTF_printNumI(write_data, x, y, num_width, UTF_RIGHT);
    }


#define DATA_SIZE 78
    static uint8_t data[DATA_SIZE];
    static uint8_t data_readed[DATA_SIZE];
    for(int i=0;i<DATA_SIZE; i++)
        data[i] = i+2;

#define OFFSETS_COUNT 4
    uint16_t offsets[OFFSETS_COUNT] = {0, 270, 500, 65536-DATA_SIZE};

    for(int offsi=0; offsi<OFFSETS_COUNT; offsi++)
    {
        m25p16_write(flash_sector, offsets[offsi], DATA_SIZE, data);
    }

    for(int offsi=0; offsi<OFFSETS_COUNT; offsi++)
    {
        m25p16_read(flash_sector, offsets[offsi], DATA_SIZE, data_readed);
        int cmp = memcmp(data, data_readed, DATA_SIZE);

        x = UTF_DrawString(x,y, "offsi=");
        x = UTF_printNumI(offsi, x, y, num_width, UTF_LEFT);
        x = UTF_DrawString(x,y, (cmp==0)?"ok":"fail");
        x = 0; y+= UTF_Height();
    }

    while(1);
}

typedef struct
{
    uint32_t data0;
    uint32_t data1;
    uint32_t data2;
    uint16_t data3;
    uint16_t data4;
} TestWriteStruct;

void TestStoreToSpi()
{
    int x = 0, y = 0;
    UTFT_clrScr();
    UTF_SetFont(font_condensed30);
    UTFT_setColorW(VGA_WHITE);
    UTFT_setBackColorW(VGA_BLACK);

    m25p16_init();
    x = UTF_DrawString(x,y, "TestStoreToSpi init");
    x = 0; y+= UTF_Height();

    m25p16_power_up_flash_ram();
    int num_width = UTF_StringWidth("0000");

    uint8_t flash_sector = 2;

    TestWriteStruct d = {0x12345678, 0x22334455, 0x77221100, 0xFF00};
    TestWriteStruct dr;

    bool ok;

    for(int i=0; i<5000; i++)
    {
        UTF_printNumI(i, x, y, num_width, UTF_LEFT);

        ok = SpiFlashWriteToFlash(flash_sector, sizeof(d), &d);
        if(!ok)
        {
            x = 0; y+= UTF_Height();
            x = UTF_DrawString(x,y, ok?"write ok":"write fail");
        }

        ok = SpiFlashReadFromFlash(flash_sector, sizeof(dr), &dr);
        if(!ok)
        {
            x = 0; y+= UTF_Height();
            x = UTF_DrawString(x,y, ok?"read ok":"read fail");
        }

        ok = memcmp(&d, &dr, sizeof(d))==0;
        if(!ok)
        {
            x = 0; y+= UTF_Height();
            x = UTF_DrawString(x,y, ok?"cmp ok":"cmp fail");
        }

        d.data4+=77;
    }

    while(1);
}
*/
