//
// Created by jiubei on 4/14/23.
//

#ifndef ROOM_H
#define ROOM_H

#include <map>
#include <set>
#include <random>
#include <vector>

#include "LogicServer.h"
#include "../Header/Message.h"

class Room
{
public:
    /* roomid, roomname, roomInitSize */
    Room(int id, std::string name, int size);

    ~Room();

public:
    void Resize(int size);

    void JoinRoom(int userid);

    void Reconnect(int userid);

    void KickAllUser();

    /* 主动离开房间（不是意外断线）*/
    void LeaveRoom(int userid);

    void ChangeRoomHost(int userid);

    void OnUserOperate(Message *message);

    void StartGame(int userid);

    void EndGame();

    void Tick();

    void NotifyGameStart();

    void NotifyGameEnd();

    void NotifyUserJoinGame(int userid);

    void NotifyUserQuitGame(int userid);

    inline std::string Name() { return m_name; }

    inline int PlayerCount() { return userid2userpid.size(); }

    inline int MaxSize() { return m_size; }

    bool IsFull() { return PlayerCount() >= MaxSize(); }

    RoomProto::RoomInfo *GetRoomListBody();

private:
    void BroadCastToAllClients(BODYTYPE type, google::protobuf::MessageLite *message);

private:
    int m_id; // 房间id

    std::string m_name; // 房间名

    FrameProto::Frame frame;

    int frame_id;

    int m_size;

    uint m_seed;

    /* 空闲的玩家pid列表 */
    std::set<int> userpid_pool;

    /* 房主userid，-1表示没有房主 */
    int host_userid;

    bool m_gameStarted;

    std::vector<FrameProto::Frame> all_frames;

    /* userid 到 user在房间中id的映射 */
    std::map<int, int> userid2userpid;
};

#endif // ROOM_H
