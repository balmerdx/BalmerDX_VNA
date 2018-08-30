#include "data_process.h"
#include "usbd_cdc_vcp.h"

#include <string.h>
#include "7bit.h"

static uint8_t packet_buffer[2][DATA_PACKET_SIZE];
static uint8_t packet_buffer_size[2] = {0, 0};
static uint8_t cur_buffer = 0;

#define VIRTUAL_COM_PORT_DATA_SIZE (DATA_PACKET_SIZE*2)

static uint8_t  USB_Tx_Buffer [VIRTUAL_COM_PORT_DATA_SIZE+2];
static uint32_t USB_Tx_length  = 0;

static Bit7EncodeContext g_encodeContext;
static Bit7DecodeContext g_decodeContext;

void DataReceiveInit()
{
    bit7encodeInit(&g_encodeContext);
    bit7decodeInit(&g_decodeContext);
}

void DataReceive(uint8_t* data, uint32_t size)
{
	uint8_t* buffer = packet_buffer[cur_buffer];
	uint8_t* buffer_size = packet_buffer_size+cur_buffer;

	for(uint32_t pos = 0; pos<size; pos++)
	{
        bit7decodeAddByte(&g_decodeContext, data[pos]);

        uint8_t byte;
        bool complete;
        if(bit7decodeGetByte(&g_decodeContext, &byte, &complete))
        {
            if(*buffer_size<DATA_PACKET_SIZE)
            {
                buffer[*buffer_size] = byte;
                ++*buffer_size;
            }
        }

        if(complete)
		{
			PacketReceive(buffer, *buffer_size);
			cur_buffer = (cur_buffer+1)%2;
			buffer = packet_buffer[cur_buffer];
			buffer_size = packet_buffer_size+cur_buffer;
			*buffer_size = 0;
			continue;
		}
	}
}

void USBFlush(void)
{
	VCP_send_buffer(USB_Tx_Buffer, USB_Tx_length);
	USB_Tx_length = 0;
}

void USBSend(void)
{
    bit7encodeEndPacket(&g_encodeContext);
    uint8_t byte;
    while(bit7encodeGetByte(&g_encodeContext, &byte))
    {
        USB_Tx_Buffer[USB_Tx_length++] = byte;
    }

	USBFlush();
}


void USBAdd(uint8_t* data, uint32_t size)
{
	for(uint32_t i=0; i<size; i++)
	{
        uint8_t byte;
        while(bit7encodeGetByte(&g_encodeContext, &byte))
        {
            USB_Tx_Buffer[USB_Tx_length++] = byte;
            if(USB_Tx_length>=VIRTUAL_COM_PORT_DATA_SIZE)
                USBFlush();
        }

        bit7encodeAddByte(&g_encodeContext, data[i]);
	}
}

void USBAddStr(char* data)
{
	USBAdd((uint8_t*)data, strlen(data));
}

void USBAdd8(uint8_t data)
{
	USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd16(uint16_t data)
{
	USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd32(uint32_t data)
{
	USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAdd32i(int32_t data)
{
	USBAdd((uint8_t*)&data, sizeof(data));
}

void USBAddF(float data)
{
	USBAdd((uint8_t*)&data, sizeof(data));	
}
