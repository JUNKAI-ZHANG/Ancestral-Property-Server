#ifndef _PROFILE_H
#define _PROFILE_H

#define MAX_CLIENTS 100
#define MAX_BUFFER_SIZE 1048576
#define HEAD_SIZE 12
#define TMP_BUFFER_SIZE 1024

/*
 * @brief
 * 服务器类型
 */
enum SERVER_TYPE
{
    NONE = 0,
    CENTER = 1,
    GATE = 2,
    LOGIC = 3,
    DATABASE = 4
};

/*
 * @brief
 * 服务器繁忙程度
 */
enum SERVER_FREE_LEVEL
{
    FREE = 0,
    COMMON = 1,
    BUSY = 2,
    DOWN = 3 //宕机
};

#endif