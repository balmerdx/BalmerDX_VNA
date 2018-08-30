#pragma once

typedef struct
{
	uint32_t prefix0;
	uint32_t prefix1;
	uint32_t version;
	char date[16];
} BootloaderVersion;

#define BOOTLOADER_PREFIX0 0x839994cc
#define BOOTLOADER_PREFIX1 0x6634c227

//Не использовать этот макрос в VNA, использовать
#define BOOTLOADER_VERSION 1

#define BOOTLOADER_FLASH_START 0x08000000
#define BOOTLOADER_FLASH_SIZE 0x08000

uint32_t BootloaderGetVersion();
char* BootloaderGetDate();

