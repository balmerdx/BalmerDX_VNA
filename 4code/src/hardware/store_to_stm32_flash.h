#pragma once

/*
 * Очень простой вариант менеждмента flash памяти.
 * На одной странице может писаться только одна структура данных.
 * Эта структура данных находится в SRAM памяти, и пишется полностью во flash.
 * Если размер структуры изменился, то данные теряются.
 * Предполагается, что размер структуры сильно меньше, чем размер страницы памяти.
 * Поэтому на странице может поместится несколько структур.
 * Они пишутся последовательно, для того, чтобы уменьшить количество erase памяти.
 * Если структура отличается от предыдущей только увеличением количества ноликов, то она пишется поверх.
 * Оставляем верхние 512 Кб для записи во flash. Это FLASH_Sector_8..FLASH_Sector_11.
 * Это сектора по 128 Кб памяти.
 * Размер структуры должен быть кратен 4 байтам.
*/

bool STM32WriteToFlash(uint16_t flash_sector, uint16_t struct_size, void* void_data);
bool STM32ReadFromFlash(uint16_t flash_sector, uint16_t struct_size, void* void_data);
bool STM32ReadFromFlashSafe(uint16_t flash_sector, uint16_t struct_size, void* void_data, uint16_t void_data_size);

//Поcледняя операция записи стерла страницу флеша.
extern bool stm32_last_write_erase;
//Поcледняя операция записи записала данные на тоже самое место.
extern bool stm32_last_write_overwrite;
extern int stm32_last_write_index;
extern int stm32_last_read_index;

int SizeofFlashDataHeader();
