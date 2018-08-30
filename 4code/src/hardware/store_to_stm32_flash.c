#include "store_to_stm32_flash.h"

typedef struct FlashDataHeader
{
    union
    {
        struct
        {
            //Индекс не может быть равен 0 или 0xFFFF
            uint16_t struct_index1;
            uint16_t struct_size;
        } d;
        uint32_t data32;
    };
} FlashDataHeader;

//test 64 byte
//#define FLASH_SECTOR_SIZE 64

bool stm32_last_write_erase = false;
bool stm32_last_write_overwrite = false;
int stm32_last_write_index = 0;
int stm32_last_read_index = 0;

int SizeofFlashDataHeader()
{
    return sizeof(FlashDataHeader);
}


static uint32_t GetFlashAddress(uint16_t flash_sector)
{
    //128 Kbytes per sector
    switch(flash_sector)
    {
    case FLASH_Sector_2: return ((uint32_t)0x08008000);
    case FLASH_Sector_3: return ((uint32_t)0x0800c000);
    case FLASH_Sector_8:  return ((uint32_t)0x08080000);
    case FLASH_Sector_9:  return ((uint32_t)0x080A0000);
    case FLASH_Sector_10: return ((uint32_t)0x080C0000);
    case FLASH_Sector_11: return ((uint32_t)0x080E0000);
    }

    return 0;
}

static uint32_t GetFlashSize(uint16_t flash_sector)
{
    //128 Kbytes per sector
    switch(flash_sector)
    {
    case FLASH_Sector_2: return 0x4000;
    case FLASH_Sector_3: return 0x4000;
    case FLASH_Sector_8:  return 0x20000;
    case FLASH_Sector_9:  return 0x20000;
    case FLASH_Sector_10: return 0x20000;
    case FLASH_Sector_11: return 0x20000;
    }

    return 0;
}

bool STM32WriteToFlash(uint16_t flash_sector, uint16_t struct_size, void* void_data)
{
    FLASH_Status status;
    stm32_last_write_erase = false;
    stm32_last_write_overwrite = false;
    stm32_last_write_index = -1;

    uint32_t* data = (uint32_t*)void_data;
    uint32_t start_address = GetFlashAddress(flash_sector);
    if(start_address==0)
        return false;
    if(struct_size%4!=0 || struct_size==0)
        return false;

    uint32_t data_size = struct_size+sizeof(FlashDataHeader);

    uint32_t structs_count = GetFlashSize(flash_sector)/data_size;
    if(structs_count==0)
        return false;
    int struct_size4 = struct_size/4;

    bool need_erase = true;
    uint32_t found_struct_index = 0;

    //Ищем последний блок с данными
    for(int struct_index=structs_count-1; struct_index>=0; struct_index--)
    {
        FlashDataHeader* h = (FlashDataHeader*)(start_address+(uint32_t)(struct_index)*data_size);
        uint32_t* flash_data = (uint32_t*)(h+1);

        if(h->d.struct_index1==0xFFFF && h->d.struct_size==0xFFFF)
        {
            //Проверяем, что все данные у нас 0xFFFFFFFF
            bool ok = true;
            for(uint32_t i=0; i<struct_size4; i++)
            {
                if(flash_data[i] != 0xFFFFFFFF)
                    ok = false;
            }

            if(ok)
                continue;
            else
                break;//Данные не пустые и не валидные, значит надо все перезаписать с нуля
        }

        if(h->d.struct_index1 != struct_index+1 || h->d.struct_size!=struct_size)
            break;

        //Проверяем, что данные в этом блоке только выставляют нули, но не ставят единиц.
        bool ok = true;
        for(uint32_t i=0; i<struct_size4; i++)
        {
            //valid
            //fd == 1 && d = 0
            //fd == 1 && d = 1
            //fd == 0 && d = 0

            //invalid
            //fd == 0 && d = 1

            if(~flash_data[i] & data[i])
            {
                ok = false;
                break;
            }
        }

        if(ok)
        {
            stm32_last_write_overwrite = true;
            found_struct_index = struct_index;
            need_erase = false;
            break;
        } else
        {
            if(struct_index<structs_count-1)
            {
                //Следующий блок точно пустой.
                need_erase = false;
                found_struct_index = struct_index + 1;
                break;
            } else
            {
                //Вся страница закончиласть, надо с начала писать.
                break;
            }

        }
    }

    if(need_erase)
    {
        stm32_last_write_erase = true;
        FLASH_Unlock();
        status = FLASH_EraseSector(flash_sector, VoltageRange_3);
        FLASH_Lock();
        if(status!=FLASH_COMPLETE)
            return false;
    }

    FlashDataHeader* h = (FlashDataHeader*)(start_address+found_struct_index*data_size);
    uint32_t* flash_data = (uint32_t*)(h+1);

    FLASH_Unlock();

    FlashDataHeader header;
    header.d.struct_index1 = found_struct_index+1;
    header.d.struct_size = struct_size;

    stm32_last_write_index = found_struct_index;

    status = FLASH_ProgramWord((uint32_t)h, header.data32);
    if(status!=FLASH_COMPLETE)
    {
        FLASH_Lock();
        return false;
    }

    for(uint32_t i=0; i<struct_size4; i++)
    {
        status = FLASH_ProgramWord((uint32_t)(flash_data+i), data[i]);
        if(status!=FLASH_COMPLETE)
        {
            FLASH_Lock();
            return false;
        }
    }

    FLASH_Lock();

    return true;
}

bool STM32ReadFromFlash(uint16_t flash_sector, uint16_t struct_size, void *void_data)
{
    return STM32ReadFromFlashSafe(flash_sector, struct_size, void_data, struct_size);
}

bool STM32ReadFromFlashSafe(uint16_t flash_sector, uint16_t struct_size, void* void_data, uint16_t void_data_size)
{
    stm32_last_read_index = -1;

    uint32_t* data = (uint32_t*)void_data;
    uint32_t start_address = GetFlashAddress(flash_sector);
    if(start_address==0)
        return false;
    if(struct_size%4!=0 || struct_size==0)
        return false;

    if(void_data_size%4!=0 || void_data_size==0)
        return false;

    if(void_data_size>struct_size)
        return false;


    uint32_t data_size = struct_size+sizeof(FlashDataHeader);

    uint32_t structs_count = GetFlashSize(flash_sector)/data_size;
    if(structs_count==0)
        return false;

    //Ищем последний блок с данными
    for(int struct_index=structs_count-1; struct_index>=0; struct_index--)
    {
        FlashDataHeader* h = (FlashDataHeader*)(start_address+ ((uint32_t)struct_index)*data_size);
        uint32_t* flash_data = (uint32_t*)(h+1);

        if(h->d.struct_index1 == struct_index+1 && h->d.struct_size==struct_size)
        {
            stm32_last_read_index = struct_index;
            int struct_size4 = void_data_size/4;
            for(uint32_t i=0; i<struct_size4; i++)
            {
                data[i] = flash_data[i];
            }
            return true;
        }
    }

    return false;
}
