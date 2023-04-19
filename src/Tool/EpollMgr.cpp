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
public:
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
        event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
        event.data.fd = fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
        {
            std::cerr << "Failed to add connection_fd to epoll instance" << std::endl;
            return -1;
        }

        return 1;
    }

};

#endif