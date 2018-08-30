#pragma once

//FLASH_START_PROGRAM_ADDR
void JumpToApp(uint32_t start_program_addr);

//Функцию надо определить, она выключает всю критическую
//периферию STM32 перед переходом на другую программу.
void DeinitPerifery();
