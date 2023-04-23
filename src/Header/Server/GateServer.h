#ifndef _GATESERVER_H
#define _GATESERVER_H

#include "FuncServer.h"

struct ClientInfo
{
    int m_userid;
    int m_fd;
    /* 上一次接收到心跳包的时间 */
    time_t m_lstMs;
    ClientInfo(int userid, int fd, time_t lstMs)
    {
        m_userid = userid;
        m_fd = fd;
        m_lstMs = lstMs;
    }
};

class GateServer : public FuncServer
{
private:

private:
    /* 尝试连接其他类型功能服务器 */
    virtual void TryToConnectAvailabeServer();

    void DisconnectAllClients();

    bool CheckMessageValid(Message *msg, int fd);
    
protected:
    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

    /* 成功连接到center server后触发 */
    virtual void OnConnectToCenterServer();

public:
    explicit GateServer();

    virtual ~GateServer();

    virtual void CloseClientSocket(int fd);

    void CheckAllClientConn();

private:
    std::map<int, ClientInfo *> m_user_connections;
};

#endif