#ifndef _ROOM_H
#define _ROOM_H

#include <queue>
#include <string>

#include "Proto.h"
#include "MessageUtils.h"

// About Room MessageFactory
class Room
{
public:
    void JoinRoom(Message *msg, int fd);

    void LeaveRoom(Message *msg, int fd);

    void CreateRoom(Message *msg, int fd);

    void GetRoomList(Message *msg, int fd);

    void UserLogout(ServerProto::UserInfo *body);

    void AddUserOperate(Message *msg, int fd);
    
    void BroadCastMsg();

    void NotifyRoomStart(Message *msg, int fd);

    void NotifyUserJoin(int userid);

    void SendMsg(Message *msg, int fd);

public :
    Room();

    Room(void (*SendMsg)(Message *, int));

    ~Room();

private:
    /* need be broadcast roomlist */
    std::set<int> broadcast_list;

    /* roomid - players */
    std::map<int, std::set<int>> room;

    /* userid - roomid */
    std::map<int, int> user_room;

    /* roomid - roomname */
    std::map<int, std::string> room_name;

    /* roomid - totFrmae */
    std::map<int, std::vector<Message *>> room_total_frame;

    /* roomid - frame(in time order) (frame per second) */
    std::map<int, std::queue<Message *>> room_frame;

    /* userid - userpid */
    std::map<int, int> userid_userpid;

    /* roomid - now framecount */
    std::map<int, int> room_framecount;

    /* user - gateclient_fd */
    std::map<int, int> user_gate;

private:
    int now_room_count = 0;
};

#endif