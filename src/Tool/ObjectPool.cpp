#ifndef _OBJECT_POOL_CPP
#define _OBJECT_POOL_CPP

#include "../Header/Tool/ObjectPool.h"

ObjectPool::ObjectPool()
{
    size = INIT_SIZE;
    for (int i = 0; i < INIT_SIZE; i++) 
    {
        Message *message = new Message();
        
        pool.push(message);
    }
}

ObjectPool::~ObjectPool()
{
    Message *tmp = nullptr;
    while (pool.size())
    {
        tmp = pool.front();
        pool.pop();

        delete tmp;
    }

    size = 0;
}

Message *ObjectPool::Get()
{
    Message *tmp = nullptr;
    if (size > 0)
    {
        size -= 1;
        tmp = pool.front();

        pool.pop();
    }
    else 
    {
        tmp = new Message();
    }
    return tmp;
}

void ObjectPool::Return(Message *msg)
{
    if (size < SAFETY_SIZE)
    {
        delete msg->head;
        delete msg->body;
        size += 1;
        pool.push(msg);
    }
    else 
    {
        delete msg;
    }
}

#endif