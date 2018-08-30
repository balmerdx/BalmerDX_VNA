#include "stm32f4xx_conf.h"
#include "usart.h"
#include "delay.h"
#include "7bit.h"

//#define USE_PB67

/*
 * Минимальный размер команды - 4 байта и состоит из
 * uint16_t size
 * uint16_t command
*/

uint8_t usart_rx[MAX_RX_LEN];
uint8_t usart_tx[BIT7_ARRAY_SIZE(MAX_TX_LEN)];
volatile uint32_t usart_tx_len;
volatile uint32_t usart_tx_pos;

uint16_t receive_time_ms;
volatile uint32_t usart_rx_len;
uint32_t usart_tx_packet_len;
volatile bool usart_packet_complete;

static Bit7EncodeContext g_encodeContext;
static Bit7DecodeContext g_decodeContext;

static bool usart_skip_if_inited = false;

void UsartSkipIfInited(bool skip)
{
    usart_skip_if_inited = skip;
}

bool UsartIsInited()
{
    return (RCC->APB2ENR & RCC_APB2Periph_USART1)?true:false;
}

void UsartInit()
{
    bit7encodeInit(&g_encodeContext);
    bit7decodeInit(&g_decodeContext);

    usart_rx_len = 0;

    usart_tx_len = 0;
    usart_tx_pos = 0;
    usart_tx_packet_len = 0;
    usart_packet_complete = false;

    if(!(usart_skip_if_inited && UsartIsInited()))
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

#ifdef USE_PB67
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
#else
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
#endif

        GPIO_InitTypeDef GPIO_InitStructure;
#ifdef USE_PB67
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;//PB6-TX, PB7-RX
#else
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;//PA9-TX,  PA10-RX
#endif
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#ifdef USE_PB67
        GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
        GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate = USART_BAUD_RATE;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
        USART_Init(USART1, &USART_InitStructure);

        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//low priority
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        USART_Cmd(USART1, ENABLE);
    }

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    //NVIC_EnableIRQ(USART1_IRQn);

    receive_time_ms = 0;
}

void UsartWaitSendEmpty()
{
    while(usart_tx_len!=0);
}

void UsartWaitSendComplete()
{
    UsartWaitSendEmpty();
    while(!(USART1->SR & USART_SR_TXE));
}

void UsartAdd(uint8_t* data, uint32_t size)
{
    UsartWaitSendEmpty();

    for(uint32_t i=0; i<size; i++)
    {
        uint8_t byte;
        while(bit7encodeGetByte(&g_encodeContext, &byte))
            usart_tx[usart_tx_packet_len++] = byte;

        bit7encodeAddByte(&g_encodeContext, data[i]);
    }

}

void UsartAdd8(uint8_t data)
{
    UsartAdd((uint8_t*)&data, sizeof(data));
}

void UsartAdd16(uint16_t data)
{
    UsartAdd((uint8_t*)&data, sizeof(data));
}

void UsartAdd32(uint32_t data)
{
    UsartAdd((uint8_t*)&data, sizeof(data));
}

void UsartAdd32i(int32_t data)
{
    UsartAdd((uint8_t*)&data, sizeof(data));
}

void UsartAdd64(uint64_t data)
{
    UsartAdd((uint8_t*)&data, sizeof(data));
}

void UsartAddF(float data)
{
    UsartAdd((uint8_t*)&data, sizeof(data));
}



void UsartSend()
{
    uint8_t byte;
    bit7encodeEndPacket(&g_encodeContext);
    while(bit7encodeGetByte(&g_encodeContext, &byte))
        usart_tx[usart_tx_packet_len++] = byte;

    if(usart_tx_packet_len==0)
        return;

    usart_tx_len = usart_tx_packet_len;
    usart_tx_pos = 0;
    usart_tx_packet_len = 0;
    while(!(USART1->SR & USART_SR_TXE));
    USART1->DR = usart_tx[0]; //send first byte
}


void USART1_IRQHandler()
{
    uint16_t USART_SR = USART1->SR;
	//Receive Data register not empty interrupt
	//if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    if(USART_SR & USART_SR_RXNE)
	{
        //USART_ClearITPendingBit(USART1, USART_IT_RXNE);

        uint8_t tmp = USART1->DR; //receive data

        uint16_t time_ms = TimeMs8();
        uint16_t dt = time_ms - receive_time_ms;
        receive_time_ms = time_ms;

        if(dt>1000)
        {
            usart_packet_complete = false;
            bit7decodeInit(&g_decodeContext);
            usart_rx_len = 0;
        }

        bit7decodeAddByte(&g_decodeContext, tmp);

        uint8_t byte;
        bool complete;
        if(bit7decodeGetByte(&g_decodeContext, &byte, &complete))
        {
            if(usart_rx_len<MAX_RX_LEN)
                usart_rx[usart_rx_len++] = byte;
        }

        if(complete)
        {
            usart_packet_complete = true;
        }
	}

	//Transmission complete interrupt
    //if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    if(USART_SR & USART_SR_TC)
	{
		usart_tx_pos++;
		if(usart_tx_pos<usart_tx_len)
		{
            USART1->DR =  usart_tx[usart_tx_pos]; //send data
		} else
		{
            USART_ClearITPendingBit(USART1, USART_IT_TC);
			usart_tx_pos = usart_tx_len = 0;
		}

    }
}

void UsartQuant()
{
    if(usart_packet_complete)
    {
        usart_packet_complete = false;
        UsartReceive(usart_rx, usart_rx_len);
        usart_rx_len = 0;
    }
}
