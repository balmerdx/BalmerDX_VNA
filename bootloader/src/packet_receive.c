#include "main.h"
#include "usart/usart.h"
#include "../4code/src/commands.h"
#include "ili/UTFT.h"
#include <string.h>
#include "write_flash.h"
#include "md5.h"

static uint32_t g_write_program_offset;
extern bool start_program;
extern int send_back_count;

void OnEraseFlash(uint32_t size, uint8_t* md5hash);
void OnWriteFlash(uint8_t* data, uint32_t data_size);

void UsartReceive(uint8_t* in_data, uint32_t in_size)
{
    uint8_t command = in_data[0];
    in_data++;
    in_size--;

    switch(command)
    {
    default:
        UsartAdd8(command);
        UsartAdd32(in_size);
        UsartSend();
        break;
    case COMMAND_PING:
        UsartAdd8(COMMAND_PING);
        UsartAdd32(*(uint32_t*)in_data);
        UsartAdd32(SELF_ID_BOOTLOADER);
        UsartSend();
        break;
    case COMMAND_START_WRITE_FLASH:
        OnEraseFlash(*(uint32_t*)in_data, in_data+4);
        break;
    case COMMAND_WRITE_FLASH:
        OnWriteFlash(in_data, in_size);
        break;
    case COMMAND_SEND_BACK:
        UsartAdd8(COMMAND_SEND_BACK);
        UsartAdd(in_data, in_size);
        send_back_count += in_size;
        UsartSend();
        break;
    }
}

bool checkProgramMd5()
{
    if(FLASH_PROGRAM_SIZE<1000)
        return false;
    if(FLASH_PROGRAM_SIZE>512*1024)
        return false;

    static MD5_CTX md5context;
    uint8_t hash[16];
    md5_init(&md5context);
    md5_update(&md5context, (uint8_t*)FLASH_START_PROGRAM_ADDR, FLASH_PROGRAM_SIZE);
    md5_final(&md5context, hash);

    bool ok = true;
    for(int i=0; i<MD5_HASH_SIZE; i++)
    {
        if(hash[i]!=FLASH_PROGRAM_MD5_HASH[i])
        {
            ok = false;
            break;
        }
    }

    return ok;
}

void OnEraseFlash(uint32_t size, uint8_t* md5hash)
{
    DrawEraseFlash(size);

    uint8_t sectors_to_erase;
    uint32_t sum_size;
    uint8_t error;

    error = 1;
    if(FlashCalcSectorsCount(size+(FLASH_START_PROGRAM_ADDR-FLASH_START_FLASH_ADDR),
                             &sectors_to_erase, &sum_size))
    {
        error = 0;
        if(FlashErase(sectors_to_erase))
            error = 0;
        else
            error = 2;
    }


    if(error==0)
    {
        error = 3;
        if(FlashWrite(0, &size, 4))
        {
            if(FlashWrite(4, (uint32_t*)md5hash, MD5_HASH_SIZE))
            {
                error = 0;
            }
        }
    }

    UsartAdd8(COMMAND_START_WRITE_FLASH);
    UsartAdd8(error);
    UsartAdd8(sectors_to_erase);
    UsartAdd32(size);
    UsartAdd32(sum_size);
    UsartSend();

    DrawWriteProgress(0, size);

    g_write_program_offset = 0;
}

//Пишет всегда блоками по 256 байт
//Возвращает MD5 сумму для зашифрованных данных
void OnWriteFlash(uint8_t *data, uint32_t data_size)
{
    uint32_t data_addr = FLASH_START_PROGRAM_ADDR + g_write_program_offset;

    if(data_size!=WRITE_TO_FLASH_SIZE ||
       data_addr+WRITE_TO_FLASH_SIZE>FLASH_END_ADDR || data_addr<FLASH_START_PROGRAM_ADDR)
    {
        UsartAdd8(COMMAND_WRITE_FLASH);
        UsartAdd8(1);
        UsartAdd32(0xFFFFFFFF);
        UsartSend();
        return;
    }

    bool ok = FlashWrite(g_write_program_offset+(FLASH_START_PROGRAM_ADDR-FLASH_START_FLASH_ADDR), (uint32_t*)data, data_size);

    uint32_t next_program_offset = g_write_program_offset + WRITE_TO_FLASH_SIZE;
    DrawWriteProgress(next_program_offset, FLASH_PROGRAM_SIZE);

    UsartAdd8(COMMAND_WRITE_FLASH);
    UsartAdd8(ok?0:2);
    UsartAdd32(g_write_program_offset);
    UsartSend();

    g_write_program_offset = next_program_offset;
    if(g_write_program_offset==FLASH_PROGRAM_SIZE)
    {
        bool ok = checkProgramMd5();
        DrawStartProgram(ok);
        start_program = ok;
    }
}

