#ifndef _LOGICSERVER_H
#define _LOGICSERVER_H

#include "FuncServer.h"

class LogicServer : public FuncServer
{
private:
    /* 尝试连接其他类型功能服务器 */
    virtual void TryToConnectAvailabeServer();

    void BroadCastMsg();

    /*
     * @brief Notify (once) room start/close game
     */
    void NotifyRoom(BODYTYPE bodytype, int roomid, int fd);

    void StartBroadCastToClient();

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

private:
    /* roomid - players */
    std::map<int, std::set<int>> room;

    /* roomid - roomname */
    std::map<int, std::string> room_name;

    /* user - frame */
    std::map<int, std::vector<Message *>> user_frame;

    /* user now frame */
    std::map<int, std::queue<Message *>> user_now_frame;

    /* user - gateclient_fd */
    std::map<int, int> user_gate;

    /* roomid - now framecount */
    std::map<int, int> room_framecount;

    /* userid - roomid */
    std::map<int, int> user_room;

    /* need be broadcast roomlist */
    std::set<int> broadcast_list;

    int now_room_count = 0;
};

#endif