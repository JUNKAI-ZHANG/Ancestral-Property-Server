#ifndef _LOGICSERVER_H
#define _LOGICSERVER_H

#include <map>

#include "../Header/Singleton.h"
#include "../Header/FuncServer.h"

class Room;
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

    virtual void Update();

public:
    LogicServer();

    virtual ~LogicServer();

    virtual void CloseClientSocket(int fd);

    void BootServer(int port);

    bool SendMsg(Message *msg, int fd);

    bool SendToClient(BODYTYPE type, google::protobuf::MessageLite *message, int userid);

    void MovePlayerToRoom(int userid, int roomid);

    void RemovePlayerFromRoom(int userid);

    void AddPlayerToRoom(int userid, int roomid);

public:// 业务逻辑
    void HandleJoinRoom(Message *msg);

    void HandleLeaveRoom(Message *msg);

    void HandleCreateRoom(Message *msg);

    void HandleGetRoomList(Message *msg);

    void HandleStartGame(Message *msg);

    void HandleCloseGame(Message *msg);

    void HandleUserOperate(Message *msg);

private:
    int AddRoom(std::string roomname);

    void RemoveRoom(int roomid);

private:
    /* roomid - Room * */
    std::map<int, Room *> rooms;

    /* 可使用的房间池 */
    std::set<int> roomid_pool;

    /* 正在使用的房间池 */
    std::set<int> roomid_using;

    const int DefaultRoomCount = 10;

    /* 保存每个user对应的roomid */
    std::map<int, int> userid2roomid; 

    /* 保存user和gate的映射 */
    std::map<int, int> user_gate;

};

#define LOGICSERVER (*Singleton<LogicServer>::get())

#endif