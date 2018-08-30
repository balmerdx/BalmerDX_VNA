#include "bootloader_version.h"

BootloaderVersion* BootloaderFind()
{
    uint32_t size4 = BOOTLOADER_FLASH_SIZE/4;
    uint32_t* data = (uint32_t*)BOOTLOADER_FLASH_START;
    for(uint32_t i=0; i<size4; i++)
    {
        if(data[i]==BOOTLOADER_PREFIX0 && data[i+1]==BOOTLOADER_PREFIX1)
        {
            return (BootloaderVersion*)(data+i);
        }
    }

    return 0;
}

uint32_t BootloaderGetVersion()
{
    BootloaderVersion* st = BootloaderFind();
    if(st==0)
        return 0;
    return st->version;
}

char* BootloaderGetDate()
{
    BootloaderVersion* st = BootloaderFind();
    if(st==0)
        return "";

    return st->date;
}
