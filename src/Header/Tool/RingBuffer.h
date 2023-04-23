#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include <stdint.h>

/*
 * @brief 环形缓存，优化服务器缓存读写，避免PopBuffer浪费服务器性能，同时简化Buffer的操作难度。
 */
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

    /*
     * @brief 向缓存区里添加一段新的Buffer
     */
    bool AddBuffer(uint8_t *buffer, uint32_t size);

    /*
     * @brief 从缓存区里去掉一段Buffer
     */
    bool PopBuffer(uint32_t size);

    /*
     * @brief 需要手动释放!!
     */
    uint8_t *GetBuffer(uint32_t len);

    /*
     * @brief 得到缓存区剩余的容量
     */
    uint32_t GetRemain();

    /*
     * @brief 得到当前缓存区大小
     */
    uint32_t GetCapacity();

    uint8_t operator[](int id);
};

#endif