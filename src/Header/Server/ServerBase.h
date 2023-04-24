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

#include "../EnumHome.h"
#include "../Tool/RingBuffer.h"
#include "../Message/MessageUtils.h"

#include "../../Tool/ThreadSafetyQueue.cpp"
#include "../../Tool/Timer.cpp"
#include "../../Tool/EpollMgr.cpp"

class MessagePair
{
public:
    MessagePair() = default;

    MessagePair(Message *_message, int _fd)
    {
        msg = _message;
        fd = _fd;
    }
public:
    Message *msg;
    int fd;
};

class ServerBase
{
protected:
    int listen_fd = -1;

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

protected:
    /* 用于和中心服务器连接 */
    int center_server_client = -1;

    int listen_port = -1;

    std::string listen_ip = "127.0.0.1";

    SERVER_TYPE server_type;

    // 所有客户端连接
    std::map<int, RingBuffer *> connections;

    EpollMgr *listen_epoll = nullptr;

    ServerProto::SERVER_TYPE TransformType(SERVER_TYPE server_type);

    void SendToCenterServerConnChange(ServerProto::SERVER_TYPE server_type, std::string ip, int port, int change);

    void SendConnsChange(ServerProto::SERVER_TYPE server_type, std::string ip, int port, int change);


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

    time_t getCurrentTime();

public:
    explicit ServerBase();

    virtual ~ServerBase();

    virtual void CloseClientSocket(int fd);

    virtual void CloseServer();

    virtual void BootServer(int port);

    bool SendMsg(Message *msg, int fd);

    void SendMsgConsumer();

    void FireAllEvents();

    void FireEvent();

protected:
    std::vector<Timer *> m_callfuncList;

    int conns_count = 0; // 服务器接收的连接数量

    ThreadSafeQueue<MessagePair> _message_queue;

};

#endif