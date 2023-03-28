#include <chrono>
#include <thread>
#include <functional>

class Timer
{
public:
    Timer() {}
    ~Timer() {}

    // Timer销毁依然可以存在，但无法stop，造成泄露
    template <typename _Callable, typename... _Args>
    void start(unsigned int interval, _Callable &&__f, _Args &&...__args)
    {
        m_running = true;

        auto func = std::bind(std::forward<_Callable>(__f),
                              std::forward<_Args>(__args)...);

        m_thread = std::thread([=](){
            while (m_running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                func();
            } 
        });
    }

    // Timer销毁依然可以存在，且线程结束自动释放
    template <typename _Callable, typename... _Args>
    void startOnce(unsigned int interval, _Callable &&__f, _Args &&...__args)
    {
        auto func = std::bind(std::forward<_Callable>(__f),
                              std::forward<_Args>(__args)...);

        m_thread = std::thread([=](){
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            func(); 
        });

        m_thread.detach();
    }

    void stop()
    {
        m_running = false;
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

private:
    std::thread m_thread;
    bool m_running{true};
};
