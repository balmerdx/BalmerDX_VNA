#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Очень простой вариант менеждмента flash памяти.
 * На одной странице может писаться только одна структура данных.
 * Эта структура данных находится в SRAM памяти, и пишется полностью во flash.
 * Если размер структуры изменился, то данные теряются.
 * Предполагается, что размер структуры сильно меньше, чем размер страницы памяти.
 * Поэтому на странице может поместится несколько структур.
 * Они пишутся последовательно, для того, чтобы уменьшить количество erase памяти.
 * Если структура отличается от предыдущей только увеличением количества ноликов, то она пишется поверх.
 * Всего есть m25p16_sectors_count секторов по m25p16_sector_size байт
*/


//flash_sector>=0 && flash_sector<m25p16_sectors_count()
bool SpiFlashWriteToFlash(uint16_t flash_sector, uint16_t struct_size, void* void_data);
bool SpiFlashReadFromFlash(uint16_t flash_sector, uint16_t struct_size, void* void_data);

#ifdef __cplusplus
} //extern "C"
#endif
