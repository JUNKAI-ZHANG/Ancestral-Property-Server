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

    std::mutex connections_mutex;

    bool isConnEpollStart = false;

    /* 临时接收缓冲区内容 */
    uint8_t tmp[TMP_BUFFER_SIZE];

protected:
    /*
     * @return 1 success -1 failed
     */
    int StartListener(int port);

    /*
     * 启动tcp server前自动创建处理连接的epoll实例
     */
    void CreateConnEpoll();

    void HandleListenerEvent(std::map<int, RingBuffer *> &, int);

    void HandleConnEvent(std::map<int, RingBuffer *> &, int);

    /*
     * 解决序列化和粘包,并在OnMsgBodyAnalysised统一处理
     */
    void HandleReceivedMsg(RingBuffer *, int);

    int64_t getCurrentTime();

    void BroadCastMsg();

protected:
    int listen_port = -1;

    SERVER_TYPE server_type;

    // 所有客户端连接
    std::map<int, RingBuffer *> connections;

    EpollMgr *listen_epoll = nullptr;

    EpollMgr *conn_epoll = nullptr;

protected:
    bool ConnectToOtherServer(std::string ip, int port, int &fd);

    bool SendMsg(Message *msg, int fd);

    /*
     * @brief 服务端成功开启监听后进行的初始化操作
     */
    virtual bool OnListenerStart();
    
    /*
     * @brief 子类可以处理自己想处理的类型, header里包含token信息需要处理
     */
    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

public:
    explicit ServerBase();

    virtual ~ServerBase();

    virtual void CloseClientSocket(int fd);

    virtual void CloseServer();

    virtual void BootServer(int port);
protected:
    /* roomid - players */
    std::map<int, std::set<int>> room;

    /* userid - roomid */
    std::map<int, int> user_room;

    /* roomid - roomname */
    std::map<int, std::string> room_name;

    /* roomid - totFrmae */
    std::map<int, std::vector<Message *>> room_total_frame;

    /* roomid - frame(in time order) (frame per second) */
    std::map<int, std::queue<Message *>> room_frame;

    /* user - gateclient_fd */
    std::map<int, int> user_gate;

    /* userid - userpid */
    std::map<int, int> userid_userpid;

    /* roomid - now framecount */
    std::map<int, int> room_framecount;

    /* need be broadcast roomlist */
    std::set<int> broadcast_list;

    int now_room_count = 0;

    int tmpx = 0;

    time_t STime = 0;
};

#endif