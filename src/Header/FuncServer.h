#ifndef _FUNCSERVER_H
#define _FUNCSERVER_H

#include "ServerBase.h"

// 功能服务器,提供业务支持
class FuncServer : public ServerBase
{
private:
    // center server config
    std::string center_ip = "127.0.0.1";
    /* 中心服务器端口 */
    int center_port = 8088;

    Timer center_connect_timer;
protected:
    /* 用于和中心服务器连接 */
    int center_server_client = -1;

    /* 用于和游戏逻辑服务器连接 */
    int logic_server_client = -1;

    /* 用于和数据库服务器连接 只用来做登录处理 */
    int db_server_client = -1;

private:
    void HandleServerInfo(Message *msg, int fd);

protected:
    /* 尝试连接其他类型功能服务器 */
    virtual void TryToConnectAvailabeServer() = 0;

    /* 申请一个服务器 */
    void ApplyServerByType(SERVER_TYPE);

    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

    /*
     * @brief 连接到center server后进行的初始化操作
     */
    virtual void OnConnectToCenterServer();

    void SendSelfInfoToCenter();

    virtual bool OnListenerStart();

public:
    explicit FuncServer();

    virtual ~FuncServer();

    virtual void CloseServer();
};

#endif