#ifndef _SERVERBASE_H
#define _SERVERBASE_H

#include <map>
#include <set>
#include <queue>
#include <random>
#include <chrono>
#include <vector>
#include <cstring>
#include <utility>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "Profile.h"
#include "RingBuffer.h"
#include "MessageUtils.h"

#include "../Tool/Timer.cpp"
#include "../Tool/EpollMgr.cpp"


class ServerBase
{
protected:
    int listen_fd = -1;

    bool isConnEpollStart = false;

    /* 临时接收缓冲区内容 */
    uint8_t tmp[TMP_BUFFER_SIZE];

protected:
    /*
     * @return 1 success -1 failed
     */
    int StartListener(int port);

    void HandleListenerEvent(std::map<int, RingBuffer *> &, int);

    void HandleConnEvent(std::map<int, RingBuffer *> &, int);

    /*
     * 解决序列化和粘包,并在OnMsgBodyAnalysised统一处理
     */
    void HandleReceivedMsg(RingBuffer *, int);

    void BroadCastMsg();

    int64_t getCurrentTime();

protected:
    int listen_port = -1;

    SERVER_TYPE server_type;

    // 所有客户端连接
    std::map<int, RingBuffer *> connections;

    EpollMgr *listen_epoll = nullptr;


protected:
    bool ConnectToOtherServer(std::string ip, int port, int &fd);


    /*
     * @brief 服务端成功开启监听后进行的初始化操作
     */
    virtual bool OnListenerStart();
    
    /*
     * @brief 子类可以处理自己想处理的类型, header里包含token信息需要处理
     */
    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

    virtual void Update();

public:
    explicit ServerBase();

    virtual ~ServerBase();

    virtual void CloseClientSocket(int fd);

    virtual void CloseServer();

    virtual void BootServer(int port);

    bool SendMsg(Message *msg, int fd);

protected:
    
    time_t STime = 0;
};

#endif