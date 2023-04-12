#ifndef _FRAME_MGR
#define _FRAME_MGR

#include <map>

#include "ObjectPool.h"

// 对于每个房间使用FrameMgr, 使得每个房间的帧信息只保留一小时，超出的帧自动删除
class FrameMgr
{
public:
    FrameMgr();

    ~FrameMgr();

    bool AddFrame(const int& frame_count, Message *message);

    bool DelFrame(const int& frame_count);

    std::vector<Message *> GetFrame(const int& frame_count);

    int GetFrameCount();

private:
    /* FrameNumber - (Message's of room) */
    std::map<int, std::vector<Message *>> frame;

    ObjectPool *pool;

    int tot_count = 0;

    int now_frame_count = 0;

    const int ONE_HOUR = 60 * 60 * 30;
};

#endif