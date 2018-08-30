#include "stm32f4xx_conf.h"
#include "jump_to_app.h"

typedef void(*pFunction)(void);
static pFunction Jump_To_Application;

void JumpToApp(uint32_t start_program_addr)
{
    DeinitPerifery();
    // Set system control register SCR->VTOR
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, start_program_addr);
    uint32_t JumpAddress = *(__IO uint32_t*) (start_program_addr + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    __set_MSP(*(__IO uint32_t*)start_program_addr);
    Jump_To_Application();
}
