#pragma once

// Нулевой сектор отведен под bootloader.
// Всегда пишем с первого сектора.
// 0 - начало программы, начинающиеся с первого сектора

//Первые два сектора по 32 кб отводим на bootloader
//Следующие два сектора по 32 кб отводим на мелкие, редкоменяемые данные

//Адрес, с которого начинаем писать во flash
//Первые 1 kбайт отводим на служебную информацию
//Размер программы 4 байта и MD5 сумму 16 байт
#define FLASH_START_FLASH_ADDR 0x08010000
#define FLASH_START_PROGRAM_ADDR 0x08010400
#define FLASH_END_ADDR 0x08100000

#define FLASH_PROGRAM_SIZE (*(uint32_t*)FLASH_START_FLASH_ADDR)
#define FLASH_PROGRAM_MD5_HASH ((uint8_t*)(FLASH_START_FLASH_ADDR+4))

//нужно для того, чтобы посестилась программа
//sectors_count - количество секторов, которые надо стереть
//sum_size - округленный размер памяти
bool FlashCalcSectorsCount(uint32_t size, uint8_t* sectors_count, uint32_t* sum_size);

//size - размер программы, определяем, сколько секторов
bool FlashErase(uint32_t sectors_to_erase);

//Запись массива данных в память
//Массив data должен указывать в оперативную память
//offset - от начала программы (должно быть кратно 4)
//data - записываемый массив
//size - размер массива в байтах (должно быть кратно 4)
bool FlashWrite(uint32_t offset, uint32_t* data, uint32_t size);
