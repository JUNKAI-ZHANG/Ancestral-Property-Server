#include "../Header/FrameMgr.h"

FrameMgr::FrameMgr()
{
    pool = new ObjectPool();
}

FrameMgr::~FrameMgr()
{
    for (auto &it : frame)
    {
        for (Message *it : it.second)
        {
            pool->Return(it);
        }
        it.second.clear();
    }
    frame.clear();
}

bool FrameMgr::AddFrame(const int& frame_count, Message *message)
{
    frame[frame_count].push_back(message);
    return DelFrame(frame_count - ONE_HOUR);
}

bool FrameMgr::DelFrame(const int& frame_count)
{
    if (frame.count(frame_count))
    {
        frame.erase(frame_count);
    }
    return true;
}

/* 使用完应该release，释放掉vector的内存 */
std::vector<Message *> FrameMgr::GetFrame(const int& frame_count)
{
    if (frame.count(frame_count))
    {
        return frame[frame_count];
    }
    else 
    {
        return std::vector<Message *>();
    }
}

int FrameMgr::GetFrameCount()
{
    return tot_count;
}