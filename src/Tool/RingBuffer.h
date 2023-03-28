#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_

#include <stdint.h>

class RingBuffer
{
public:
    uint8_t *_buffer;
    uint32_t _begin;
    uint32_t _end;

    /* 剩余空间 */
    uint32_t _remain;
    /* 当前已使用 */
    uint32_t _capacity;

public:
    /*
     * @brief default construct will distribute MAX_BUFFER_SIZE
     */
    RingBuffer();

    ~RingBuffer();

    bool AddBuffer(uint8_t *buffer, uint32_t size);

    bool PopBuffer(uint32_t size);

    /*
     * @brief 需要手动释放!!
     */
    uint8_t *GetBuffer(uint32_t len);

    uint32_t GetRemain();

    uint32_t GetCapacity();

    uint8_t operator[](int id);
};

#endif