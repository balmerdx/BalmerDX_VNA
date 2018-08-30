#include "ring_buffer.h"

void rb_init(RB_DATA* rb, uint8_t* buffer, uint16_t capacity)
{
    rb->buffer = buffer;
    rb->size = 0;
    rb->capacity = capacity;
    rb->read_pos = 0;
    rb->write_pos = 0;
}

bool rb_isEmpty(RB_DATA* rb)
{
    return rb->size==0;
}

bool rb_isFull(RB_DATA* rb)
{
    return rb->size>=rb->capacity;
}

bool rb_put(RB_DATA* rb, uint8_t item)
{
    if(rb_isFull(rb))
    {
        //error!!!
        return false;
    }

    rb->size++;
    rb->buffer[rb->write_pos] = item;
    rb->write_pos++;
    if(rb->write_pos>=rb->capacity)
        rb->write_pos = 0;

    return true;
}

bool rb_get(RB_DATA* rb, uint8_t* item)
{
    if(rb_isEmpty(rb))
    {
        //error
        return false;
    }

    rb->size--;
    *item = rb->buffer[rb->read_pos];
    rb->read_pos++;
    if(rb->read_pos>=rb->capacity)
        rb->read_pos = 0;

    return true;
}

bool rb_putArr(RB_DATA* rb, const uint8_t* items, uint16_t size)
{
    if(rb->size+size >= rb->capacity)
    {
        return false;
    }

    if(rb->write_pos+size > rb->capacity)
    {
        int copy_size = rb->capacity-rb->write_pos;
        for(int i=0;i<copy_size;i++)
        {
            rb->buffer[rb->write_pos++] = *items++;
        }

        size -= copy_size;
        rb->size += copy_size;
        rb->write_pos = 0;
    }

    for(int i=0;i<size;i++)
    {
        rb->buffer[rb->write_pos++] = *items++;
    }
    if(rb->write_pos>=rb->capacity)
        rb->write_pos = 0;

    rb->size += size;

    return true;
}

bool rb_getArr(RB_DATA* rb, uint8_t* items, uint16_t size)
{
    if(rb->size < size)
    {
        return false;
    }

    if(rb->read_pos+size > rb->capacity)
    {
        int copy_size = rb->capacity-rb->read_pos;
        for(int i=0;i<copy_size;i++)
        {
            *items++ = rb->buffer[rb->read_pos++];
        }

        size -= copy_size;
        rb->size -= copy_size;
        rb->read_pos = 0;
    }

    for(int i=0;i<size;i++)
    {
        *items++ = rb->buffer[rb->read_pos++];
    }

    if(rb->read_pos>=rb->capacity)
        rb->read_pos = 0;

    rb->size -= size;
    return true;
}
