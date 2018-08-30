#include "main.h"
#include "delay.h"
#include "stm32f4xx_it.h"
#include "usart/usart.h"
#include <string.h>

#include "jump_to_app.h"
#include "write_flash.h"
#include "quadrature_encoder.h"
#include "bootloader_version.h"


BootloaderVersion g_version = {
    BOOTLOADER_PREFIX0,
    BOOTLOADER_PREFIX1,
    BOOTLOADER_VERSION,
    __DATE__
};


RCC_ClocksTypeDef RCC_Clocks;

bool start_program = false;
int send_back_count = 0;

int g_info_string_y = 80;
int g_info_string_x = 64;

char str_buffer[256];

bool QuadButtonPressed()
{
    return QuadEncButton1() || QuadEncButton2();
}



int main(void)
{  
    QuadEncInit();

    if(!UsartIsInited())
    if(!QuadButtonPressed() && checkProgramMd5())
        JumpToApp(FLASH_START_PROGRAM_ADDR);

    // SysTick end of count event each 1ms
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

    DelayInit();

    UTFT_InitLCD(UTFT_LANDSCAPE);

    int r = 64;
    if(FLASH_OB_GetRDP())
        UTFT_setBackColor(r, r, r*2);
    else
        UTFT_setBackColor(r*2, r, r);
    UTFT_setColorW(VGA_WHITE);
    UTFT_clrScr();

    UsartSkipIfInited(true);
    UsartInit();
    DelayMs(50);


    //UTF_SetFont(font_condensed30);
    UTFT_setFont(FONT8x15);

    UTF_DrawStringJustify(0, 48, "VNA Bootloader ", UTFT_getDisplayXSize(), UTF_CENTER);

    g_info_string_y = (UTFT_getDisplayYSize()-UTF_Height())/2;
    UTF_DrawStringJustify(0, g_info_string_y, "Wait for programm...", UTFT_getDisplayXSize(), UTF_CENTER);

    //last string
    {
        int y = UTFT_getDisplayYSize() - UTF_Height();
        int x = 20;
        x = UTF_DrawStringJustify(x, y, "ver=", 0, UTF_LEFT);
        x = UTF_printNumI(g_version.version, x, y, 0, UTF_LEFT);
        x = UTF_DrawStringJustify(x, y, " date=", 0, UTF_LEFT);
        x = UTF_DrawStringJustify(x, y, g_version.date, 0, UTF_LEFT);
    }

    if(0)
    {
        int y = 48+UTF_Height();
        UTF_DrawStringJustify(0, y, " !\"#$%&'()*+,-./", UTFT_getDisplayXSize(), UTF_CENTER); y+=UTF_Height();
        UTF_DrawStringJustify(0, y, "0123456789:;<=>?", UTFT_getDisplayXSize(), UTF_CENTER); y+=UTF_Height();
        UTF_DrawStringJustify(0, y, "@ABCDEFGHIJKLMNO", UTFT_getDisplayXSize(), UTF_CENTER); y+=UTF_Height();
        UTF_DrawStringJustify(0, y, "PQRSTUVWXYZ[\\]^_", UTFT_getDisplayXSize(), UTF_CENTER); y+=UTF_Height();
        UTF_DrawStringJustify(0, y, "`abcdefghijklmno", UTFT_getDisplayXSize(), UTF_CENTER); y+=UTF_Height();
        UTF_DrawStringJustify(0, y, "pqrstuvwxyz{|}~ ", UTFT_getDisplayXSize(), UTF_CENTER); y+=UTF_Height();
    }

    RCC_ClocksTypeDef RCC_ClocksStatus;
    RCC_GetClocksFreq(&RCC_ClocksStatus);

    //while(1);

    while(1)
    {
        DelayMs(1);
        UsartQuant();

        if(start_program)
        {
            UsartWaitSendComplete();

            if(start_program)
            {
                //JumpToApp(FLASH_START_PROGRAM_ADDR);
                NVIC_SystemReset();
            }
        }
    }
}

void DeinitPerifery()
{
    //__disable_irq();

    //clock
    TIM_Cmd(TIM2, DISABLE);
    TIM_Cmd(TIM3, DISABLE);
    TIM_Cmd(TIM6, DISABLE);
    TIM_Cmd(TIM7, DISABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, DISABLE);

    //USART
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
    //USART_DeInit(USART1);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);

    //GPIO
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, DISABLE);
}

void AppendIntToString(char* str, int num)
{
    char st[27];
    intToString(st, num, 0, ' ');
    strcat(str, st);
}


void DrawEraseFlash(uint32_t program_size)
{
    strcpy(str_buffer, "Start erase flash. size=");
    AppendIntToString(str_buffer, program_size);
    UTF_DrawStringJustify(0, g_info_string_y, str_buffer, UTFT_getDisplayXSize(), UTF_CENTER);
}

void DrawStartProgram(bool ok)
{
    if(ok)
        UTF_DrawStringJustify(0, g_info_string_y, "Starting program", UTFT_getDisplayXSize(), UTF_CENTER);
    else
        UTF_DrawStringJustify(0, g_info_string_y, "Checksum failed", UTFT_getDisplayXSize(), UTF_CENTER);
}

void DrawWriteProgress(uint32_t offset, uint32_t program_size)
{
    static uint16_t prev_time;
    if(offset==0)
    {
        prev_time = TimeMs8();
    } else
    {
        uint16_t cur_time = TimeMs8();
        uint16_t delta = cur_time-prev_time;
        if(delta<100*8)
            return;
        prev_time = cur_time;
    }

    strcpy(str_buffer, "Flash ");
    AppendIntToString(str_buffer, offset);
    strcat(str_buffer, " / ");
    AppendIntToString(str_buffer, program_size);
    UTF_DrawStringJustify(0, g_info_string_y, str_buffer, UTFT_getDisplayXSize(), UTF_CENTER);
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

  
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
