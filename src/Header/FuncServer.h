#ifndef _FUNCSERVER_H
#define _FUNCSERVER_H

#include "ServerBase.h"

// 功能服务器,提供业务支持
class FuncServer : public ServerBase
{
protected:
    /* 用于和中心服务器连接 */
    int center_server_client = -1;

    /* 用于和游戏逻辑服务器连接 */
    int logic_server_client = -1;

    /* 用于和数据库服务器连接 只用来做登录处理 */
    int db_server_client = -1;

    /* 记录userid和fd的映射 */
    std::map<int, int> user_fd_record;
    
    /* 记录fd和userid的映射 */
    std::map<int, int> fd_user_record;

private:
    void HandleServerInfo(Message *msg, int fd);

    void HandleUserInfo(Message *msg, int fd);

protected:
    void SendSelfInfoToCenter();

    /* 申请一个服务器 */
    void ApplyServerByType(SERVER_TYPE);

    /* 尝试连接其他类型功能服务器 */
    virtual void TryToConnectAvailabeServer() = 0;

    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

    /* 连接到center server后进行的初始化操作 */
    virtual void OnConnectToCenterServer();

    virtual bool OnListenerStart();

    virtual void Update();

public:
    explicit FuncServer();

    virtual ~FuncServer();

    virtual void CloseServer();
};

#endif