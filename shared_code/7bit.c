#include "7bit.h"

void bit7encodeInit(Bit7EncodeContext* data)
{
    data->full_byte = 0;
    data->rest_byte = 0;
    data->rest_bit_count = 0;
    data->last = false;
    data->full = false;
}


void bit7encodeAddByte(Bit7EncodeContext* data, uint8_t byte)
{
    uint16_t buf = data->rest_byte | (((uint16_t)byte)<<data->rest_bit_count);
    data->full_byte = buf&0x7F;
    data->rest_bit_count++;
    data->rest_byte = buf>>7;
    data->full = true;
    data->last = false;
}

void bit7encodeEndPacket(Bit7EncodeContext *data)
{
    data->last = true;
}

bool bit7encodeGetByte(Bit7EncodeContext* data, uint8_t* byte)
{
    if(data->full)
    {
        *byte = data->full_byte;
        if(data->rest_bit_count==7)
        {
            data->full_byte = data->rest_byte;
            data->rest_byte = 0;
            data->rest_bit_count = 0;
        } else
        {
            data->full = false;
            if(data->last && data->rest_bit_count==0)
            {
                *byte |= 0x80;
                bit7encodeInit(data);
            }
        }

        return true;
    }

    if(data->last && data->rest_bit_count>0)
    {
        *byte = data->rest_byte | 0x80;
        bit7encodeInit(data);
        return true;
    }

    return false;
}


void bit7decodeInit(Bit7DecodeContext* data)
{
    data->data = 0;
    data->bit_count = 0;
    data->last = 0;
}

void bit7decodeAddByte(Bit7DecodeContext* data, uint8_t byte)
{
    data->data |= (((uint16_t)byte)<<data->bit_count);
    data->bit_count += 7;
    data->last = byte&0x80?true:false;
}

bool bit7decodeGetByte(Bit7DecodeContext* data, uint8_t* byte, bool* complete)
{
    *complete = false;
    if(data->bit_count<8)
        return false;

    *byte = data->data;
    data->bit_count -= 8;
    data->data >>= 8;
    if(data->last && data->bit_count<8)
    {
        *complete = true;
        bit7decodeInit(data);
    }

    return true;
}
