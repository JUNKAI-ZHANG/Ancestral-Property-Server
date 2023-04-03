#ifndef _EPOLLMGR_H_
#define _EPOLLMGR_H_

#include <iostream>
#include <unistd.h>
#include <functional>
#include <sys/epoll.h>

#include "../Header/Profile.h"

/*
 * Epoll 实例管理类
 *
 * 可自动回收关闭epoll_fd
 *
 * 可自定义epoll事件处理函数
 */
class EpollMgr
{
private:
    int epoll_fd = -1;

public:
    explicit EpollMgr()
    {
    }

    virtual ~EpollMgr()
    {
        // 关闭 epoll 实例
        close(epoll_fd);
    }

public:
    /*
     * @return 1 success -1 failed
     */
    int CreateEpoll()
    {
        // 创建 epoll 实例
        // Since Linux 2.6.8, the size argument is ignored, but must be greater than zero;
        epoll_fd = epoll_create(1);
        if (epoll_fd == -1)
        {
            std::cerr << "Failed to create epoll instance" << std::endl;
            return -1;
        }

        return 1;
    }

    /*
     * @return 1 success -1 failed
     */
    int AddEventToEpoll(int fd)
    {
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
        {
            std::cerr << "Failed to add connection_fd to epoll instance" << std::endl;
            return -1;
        }

        return 1;
    }

    /*
     * @param 需传入处理epoll事件的函数
     *
     * param1 function address
     *
     * param2 params
     *
     */
    template <typename _Callable, typename... _Args>
    void WaitEpollEvent(_Callable &&__f, _Args &&...__args)
    {
        // 等待连接和数据
        struct epoll_event events[MAX_CLIENTS];

        while (true)
        {
            // 阻塞等待
            int nfds = epoll_wait(epoll_fd, events, 10, -1);
            if (nfds == -1)
            {
                std::cerr << "Failed to wait for events" << std::endl;
                break;
            }

            for (int i = 0; i < nfds; i++)
            {
                // address client socket data
                int conn_fd = events[i].data.fd;

                std::bind(std::forward<_Callable>(__f),
                          std::forward<_Args>(__args)..., conn_fd)();

            }
        }

        // 关闭 epoll 实例
        close(epoll_fd);
    }
};

#endif