#ifndef _COMMANDS_H_
#define _COMMANDS_H_

typedef enum USB_COMMANDS
{
    COMMAND_BAD = -1,

    //VNA & bootloader commands
    COMMAND_PING = 0,

    //VNA commands
	COMMAND_BIG_DATA = 1,
	COMMAND_SET_FREQ = 2,
	COMMAND_START_SAMPLING = 3,
	COMMAND_SAMPLING_COMPLETE = 4,
	COMMAND_SAMPLING_BUFFER_SIZE = 5,
	COMMAND_GET_SAMPLES = 6,
	COMMAND_SET_TX = 7,
	COMMAND_GET_CALCULATED = 8,
	COMMAND_START_SAMPLING_AND_CALCULATE = 9,
    COMMAND_SET_RELATIVE_LEVEL_RF = 10, //Уровень сигнала, подаваемый на деталь. 100 - максимальный уровень.
    COMMAND_GET_RELATIVE_LEVEL_RF = 11, //Уровень сигнала, подаваемый на деталь. 100 - максимальный уровень.
    COMMAND_SET_FREQ_LIST = 12,
    COMMAND_JUMP_TO_BOOTLOADER = 13,
    COMMAND_GET_CALIBRATION = 14,
    COMMAND_GET_CALIBRATION_NAME = 15,

    //botloader commands
    COMMAND_START_WRITE_FLASH = 128,
    COMMAND_WRITE_FLASH = 129,
    COMMAND_SEND_BACK = 130,
} USB_COMMANDS;

#define SELF_ID_VNA 0xF008492Au
#define SELF_ID_BOOTLOADER 0xB001FAE0u

//Количество байт, которые надо подавать в команду COMMAND_WRITE_FLASH
#define WRITE_TO_FLASH_SIZE 256

//Для COMMAND_GET_CALIBRATION
//В ответ на этот id посылается список из соответствующих элементов
//Рассчитываем на буфер 4096 байт в который без проблемм поместятся наши 221 элементов
//221*sizeof(complexf) = 1768 байт + несколько байт инфы дополнительной.
typedef enum GET_CALIBRATION_ENUM
{
    GET_CALIBRATION_FREQ,//Первое должно быть
    GET_CALIBRATION_S11_OPEN,//Второе должно быть
    GET_CALIBRATION_S11_SHORT,
    GET_CALIBRATION_S11_LOAD,
    GET_CALIBRATION_S11_THRU,
    GET_CALIBRATION_S21_THRU,
    GET_CALIBRATION_S21_OPEN, //Последнее должно быть
} GET_CALIBRATION_ENUM;


#define MAX_RELATIVE_LEVEL_RF 100

#endif//_COMMANDS_H_
