#include <stdint.h>
#include <string.h>
#include <iostream>
#include <memory>

#include "RingBuffer.h"
#include "../Header/profile.h"

RingBuffer::RingBuffer()
{
    _buffer = new uint8_t[MAX_BUFFER_SIZE];
    _begin = _end = _capacity = 0;
    _remain = MAX_BUFFER_SIZE;
}

RingBuffer::~RingBuffer()
{
    std::cout << "Ring Deconstruct" << std::endl;
    _begin = _end = _capacity = 0;
    _remain = MAX_BUFFER_SIZE;
    delete _buffer;
}

bool RingBuffer::AddBuffer(uint8_t *buffer, uint32_t size)
{
    if (size > _remain)
    {
        return false;
    }

    for (uint32_t i = 0; i < size; i++)
    {
        _buffer[_end] = buffer[i];
        _end = (_end + 1) % MAX_BUFFER_SIZE;
    }

    _capacity += size;
    _remain -= size;

    return true;
}

bool RingBuffer::PopBuffer(uint32_t size)
{
    if (size > _capacity)
    {
        return false;
    }

    _begin = (_begin + size) % MAX_BUFFER_SIZE;

    _capacity -= size;
    _remain += size;
    return true;
}

uint8_t *RingBuffer::GetBuffer(uint32_t len)
{
    uint32_t start = _begin;
    uint8_t *ret = new uint8_t[len];

    for (uint32_t i = 0; i < len; i++)
    {
        ret[i] = _buffer[start];
        start = (start + 1) % MAX_BUFFER_SIZE;
    }

    return ret;
}

uint32_t RingBuffer::GetRemain()
{
    return _remain;
}

uint32_t RingBuffer::GetCapacity()
{
    return _capacity;
}

uint8_t RingBuffer::operator[](int id)
{
    if (id < 0 || id >= MAX_BUFFER_SIZE)
        return 0;
    return _buffer[id];
}