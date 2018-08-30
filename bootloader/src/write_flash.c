#include "stm32f4xx_conf.h"
#include "write_flash.h"

typedef struct FlashInfo
{
    uint16_t sector;
    uint32_t size;
} FlashInfo;

//#define SECTORS_COUNT 10
#define SECTORS_COUNT 8

static const FlashInfo g_sectors[SECTORS_COUNT] =
{
//    {FLASH_Sector_2, 0x4000},
//    {FLASH_Sector_3, 0x4000},
    {FLASH_Sector_4, 0x10000},
    {FLASH_Sector_5, 0x20000},
    {FLASH_Sector_6, 0x20000},
    {FLASH_Sector_7, 0x20000},
    {FLASH_Sector_8, 0x20000},
    {FLASH_Sector_9, 0x20000},
    {FLASH_Sector_10, 0x20000},
    {FLASH_Sector_11, 0x20000},
};

bool FlashCalcSectorsCount(uint32_t size, uint8_t* p_sectors_to_erase, uint32_t* p_size_to_erase)
{
    *p_sectors_to_erase = 0;
    *p_size_to_erase = 0;
    if(size==0)
        return true;

    uint32_t flash_size = 0;
    int sectors_to_erase = 0;
    for(int i=0; i<SECTORS_COUNT; i++)
    {
        flash_size += g_sectors[i].size;
        if(flash_size >= size)
        {
            sectors_to_erase = i+1;
            break;
        }
    }

    if(sectors_to_erase==0)
        return false;//overflow

    *p_sectors_to_erase = sectors_to_erase;
    *p_size_to_erase = flash_size;
    return true;
}

bool FlashErase(uint32_t sectors_to_erase)
{
    if(sectors_to_erase==0)
        return true;
    if(sectors_to_erase>SECTORS_COUNT)
        return false;

    __disable_irq();
    FLASH_Status status;
    for(int i=0; i<sectors_to_erase; i++)
    {
        FLASH_Unlock();
        status = FLASH_EraseSector(g_sectors[i].sector, VoltageRange_3);
        FLASH_Lock();
        if(status!=FLASH_COMPLETE)
        {
            __enable_irq();
            return false;
        }
    }

    __enable_irq();
    return true;
}

bool FlashWrite(uint32_t offset, uint32_t *data, uint32_t size)
{
    __disable_irq();
    FLASH_Unlock();

    for(uint32_t i=0; i<size; i+=4)
    {
        FLASH_Status status = FLASH_ProgramWord(FLASH_START_FLASH_ADDR+offset+i, data[i>>2]);
        if(status!=FLASH_COMPLETE)
        {
            FLASH_Lock();
            __enable_irq();
            return false;
        }
    }

    FLASH_Lock();
    __enable_irq();
    return true;
}
