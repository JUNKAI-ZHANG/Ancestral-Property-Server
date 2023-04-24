#include <chrono>
#include <functional>

#include "../Header/EnumHome.h"

// 定时完成委托类，new完记得调用->Start()
class Timer
{
public:
    Timer() 
    {

    }
    Timer(time_t interval, CallbackType type, std::function<void()> func) 
    {
        m_interval = interval;
        m_callbackType = type;
        m_callback = func;
    }
    ~Timer() 
    {

    }

    void Tick()
    {
        if (!m_already)
        {
            return;
        }
        time_t now_time = getCurrentTime();
        if (now_time - m_preTime >= m_interval)
        {
            if (m_onceFlag)
            {
                Stop();
            }
            m_callback();
            m_preTime = now_time;
        }
    }

    CallbackType GetType()
    {
        return m_callbackType;
    }

    void Start()
    {
        m_already = true;
    }

    /* 设置函数只需要执行一次 */
    void SetOnce()
    {
        m_onceFlag = true;
    }

    void Stop()
    {
        m_already = false;
    }

    /* 返回当前时间戳的毫秒值 */
    time_t getCurrentTime() 
    {
        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        return now_ms.time_since_epoch().count();
    }

private:
    /* 回调函数执行间隔时间 */
    time_t m_interval = 0;

    /* 上一次执行回调函数的时间戳毫秒值 */
    time_t m_preTime = 0;

    /* 是否已经执行完了一次 */
    bool m_isOk = false;

    /* 标志这个函数只需要执行一次 */
    bool m_onceFlag = false;

    /* 表示这个函数已经具备执行的资格了 */
    bool m_already = false;

    /* 回调函数 */
    std::function<void()> m_callback;

    CallbackType m_callbackType;
};
