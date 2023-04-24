#include "../Header/Tool/FrameMgr.h"

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
    // return DelFrame(frame_count - ONE_HOUR);
    return true;
}

bool FrameMgr::DelFrame(const int& frame_count)
{
    if (frame.count(frame_count))
    {
        for (Message *it : frame[frame_count]) 
        {
            pool->Return(it);
        }
        frame.erase(frame_count);
    }
    return true;
}

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