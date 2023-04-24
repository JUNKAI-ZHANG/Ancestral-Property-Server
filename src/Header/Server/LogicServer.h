#ifndef _LOGICSERVER_H
#define _LOGICSERVER_H

#include <map>

#include "../Tool/Singleton.h"
#include "FuncServer.h"

class Room; // 前置定义解决循环引用
class LogicServer : public FuncServer
{
private:
    /* 尝试连接其他类型功能服务器 */
    virtual void TryToConnectAvailabeServer();

protected:
    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

    /* 成功连接到center server后触发 */
    virtual void OnConnectToCenterServer();

    virtual void Update();

public:
    LogicServer();

    virtual ~LogicServer();

    virtual void CloseClientSocket(int fd);

    void BootServer(std::string ip, int port, std::string name);

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

    void HandleJoinGame(Message *msg);

    void HandleQuitGame(Message *msg);

    void HandleRoomChangeRole(Message *msg);

    void HandleReconnect(int userid);

    /* 从房间到游戏*/
    void HandleStartGame(Message *msg);

    /* 从游戏到房间 */
    void HandleEndGame(Message *msg);

    void HandleUserOperate(Message *msg);

    std::string GetUserName(int userid);

    bool CheckUserInRoom(int userid);

    void RemoveUser(int userid);

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

    /* 保存每个user对应的roomid */
    std::map<int, int> userid2roomid; 

    /* 保存user和gate的映射 */
    std::map<int, int> user_gate;

    std::unordered_set<int> inGames;

    /* 保存userid对应的username */
    std::map<int, std::string> userid2username;

};

#define LOGICSERVER (*Singleton<LogicServer>::get())

#endif