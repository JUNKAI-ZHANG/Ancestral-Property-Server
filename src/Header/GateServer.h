#ifndef _GATESERVER_H
#define _GATESERVER_H

#include "FuncServer.h"

class GateServer : public FuncServer
{
private:
    /* 记录userid和fd的映射 */
    std::map<std::string, int> user_fd_record;

private:
    /* 尝试连接其他类型功能服务器 */
    virtual void TryToConnectAvailabeServer();

    void DisconnectAllClients();
    
protected:
    virtual void OnMsgBodyAnalysised(Header head,
                                     const uint8_t *body,
                                     uint32_t length,
                                     int fd);

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