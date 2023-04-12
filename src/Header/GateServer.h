#ifndef _GATESERVER_H
#define _GATESERVER_H

#include "FuncServer.h"

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

    /*
     * @brief 成功连接到center server后触发
     */
    virtual void OnConnectToCenterServer();

public:
    explicit GateServer();

    virtual ~GateServer();

    virtual void CloseClientSocket(int fd);
};

#endif