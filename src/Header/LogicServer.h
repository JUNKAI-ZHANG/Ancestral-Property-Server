#ifndef _LOGICSERVER_H
#define _LOGICSERVER_H

#include "FuncServer.h"

class LogicServer : public FuncServer
{
private:
    /* 尝试连接其他类型功能服务器 */
    virtual void TryToConnectAvailabeServer();

protected:
    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

    /*
     * @brief 成功连接到center server后触发
     */
    virtual void OnConnectToCenterServer();

public:
    explicit LogicServer();

    virtual ~LogicServer();
    
    virtual void CloseClientSocket(int fd);
};

#endif