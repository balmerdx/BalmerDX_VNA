#pragma once
#define MAX_RX_LEN 4096
#define MAX_TX_LEN 4096

//undef macros to disable. Dont set 0 in macros!

#define USART_INIT_RX 1

//#define USART_BAUD_RATE 9600
//#define USART_BAUD_RATE 115200
#define USART_BAUD_RATE 500000
//#define USART_BAUD_RATE 460800
//#define USART_BAUD_RATE 230400


//Skip USART initialization if already inited
void UsartSkipIfInited(bool skip);

//flags see USART_INIT_...
void UsartInit();

bool UsartIsInited();

void UsartAdd(uint8_t* data, uint32_t size);
void UsartAdd8(uint8_t data);
void UsartAdd16(uint16_t data);
void UsartAdd32(uint32_t data);
void UsartAdd32i(int32_t data);
void UsartAdd64(uint64_t data);
void UsartAddF(float data);

void UsartSend();

void UsartWaitSendComplete();

void UsartReceive(uint8_t* in_data, uint32_t in_size);//receive data

//Вызывать в основном потоку, и из него может вызваться UsartReceive
void UsartQuant();
