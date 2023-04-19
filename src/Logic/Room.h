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

    /* 如果游戏已经开始那么不可以join */
    void JoinRoom(int userid);

    /* 从房间加入到游戏 */
    void JoinGame(int userid);

    /* 从房间进入已经开始的游戏需要调用 */
    void Reconnect(int userid);

    /* 踢掉游戏里的所有玩家 */
    void KickAllUserFromGame();

    /* 踢掉房间里的所有玩家 */
    void KickAllUserFromRoom();

    /* 大退（直接退游戏 & 退房间） */
    void Leave(int userid);

    /* 主动离开房间 */
    void LeaveFromRoom(int userid);

    /* 主动离开游戏，返回到房间中*/
    void LeaveFromGame(int userid);

    void ChangeRoomHost(int userid);

    void OnUserOperate(Message *message);

    /* 开始房间的游戏，转到游戏状态 */
    void StartGame(int userid);

    /* 结束游戏，所有玩家回到房间 */
    void EndGame();

    void SetUserOffline(int userid);

    void SetUserOnline(int userid);

    /* 需要改成对房间广播 */
    void Tick();

    void NotifyGameStartToUser(int userid);

    void NotifyGameStart();

    void NotifyGameEnd();

    /* 通知大家某玩家加入游戏，这个消息随frame下发 */
    void NotifyUserJoinGame(int userid);

    /* 通知大家某玩家退出游戏，这个消息随frame下发 */
    void NotifyUserQuitGame(int userid);

    inline std::string Name() { return m_name; }

    inline int PlayerCount() { return userid2userpid.size(); }

    inline int MaxSize() { return m_size; }

    bool IsFull() { return PlayerCount() >= MaxSize(); }

    bool CheckRoomDead();

    std::vector<RoomProto::UserInfo> GetRoomUserInfos();

private:
    void BroadCastToGame(BODYTYPE type, google::protobuf::MessageLite *message);

    void BroadCastToRoom(BODYTYPE type, google::protobuf::MessageLite *message);

private:
    int m_id; // 房间id

    std::string m_name; // 房间名

    FrameProto::Frame frame; // 当前帧

    int frame_id; // 累计帧数

    int m_size; // 房间大小

    uint m_seed; // 房间种子

    bool m_gameStarted; // 标记是否开始了游戏

    /* 房主userid，-1表示没有房主 */
    int host_userid;

    /* 空闲的玩家pid列表 */
    std::set<int> userpid_pool;

    /* 房间累计帧 */
    std::vector<FrameProto::Frame> all_frames;

    /* userid 到 user在房间中id的映射 */
    std::map<int, int> userid2userpid;

    /* 在游戏里的玩家 */
    std::map<int, int> in_game_id2pid;

    /* userid - username */
    std::map<int, std::string> id2name;

    // 掉线用户userid
    std::set<int> offline_userid;
};

#endif // ROOM_H
