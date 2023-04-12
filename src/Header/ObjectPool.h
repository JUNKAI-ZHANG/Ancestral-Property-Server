#ifndef _OBJECT_POOL
#define _OBJECT_POOL

#include <queue>
#include <utility>
#include <stdint.h>

#include "Message.h"

/* Manage Message(Head is Ok)(Body need new) */
class ObjectPool
{
public:
    ObjectPool();

    ~ObjectPool();

    Message *Get();

    void Return(Message *msg);

private:
    std::queue<Message *> pool;

    int size = 0;

    const int INIT_SIZE = 131072;

    const int SAFETY_SIZE = 262144;
};

#endif