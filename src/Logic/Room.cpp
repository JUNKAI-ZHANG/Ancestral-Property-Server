//
// Created by jiubei on 4/14/23.
//

#include "Room.h"

Room::Room(int id, std::string name, int size)
{
    m_id = id;
    m_name = name;
    m_size = size;
    for (int i = 1; i <= size; i++)
    {
        userpid_pool.insert(i);
    }
    std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    m_seed = rng() % UINT32_MAX;
    host_userid = -1;
    m_gameStarted = false;
}

Room::~Room()
{
    EndGame();
    KickAllUser();
}

void Room::Resize(int size)
{
    if (size < m_size)
    {
       for (int i = size + 1; i <= m_size; i++)
       {
           LeaveRoom(i);
           userpid_pool.erase(i);
       }
    }
    else
    {
        for (int i = m_size + 1; i <= size; i++)
        {
            userpid_pool.insert(i);
        }
    }
    m_size = size;
}

void Room::JoinRoom(int userid)
{
    RoomProto::JoinRoom message;
    message.set_type(RoomProto::JoinRoom_Type_RESPONSE);
    message.set_ret(false);
    message.set_roomid(-1);
    if (IsFull())
    {
        message.set_result("房间已满");
    }
    else if (userid2userpid.count(userid))
    {
        message.set_result("你已在此房间");
    }
    else
    {
        int pid = *userpid_pool.begin();
        userpid_pool.erase(pid);
        userid2userpid[userid] = pid;

        message.set_roomid(m_id);
        message.set_ret(true);
        message.set_result("加入成功");

        if (host_userid == -1) ChangeRoomHost(userid);
        if (m_gameStarted) NotifyUserJoinGame(userid);
    }

    LOGICSERVER.SendToClient(BODYTYPE::JoinRoom, &message, userid);
}

void Room::Reconnect(int userid)
{
    // todo 发送追帧信息
    // 1. 先发start game通知
    // 2. 把all_frames的东西发出去
}

void Room::LeaveRoom(int userid)
{
    RoomProto::LeaveRoom message;
    message.set_type(RoomProto::LeaveRoom_Type_RESPONSE);
    message.set_ret(false);
    message.set_roomid(0);
    if (!userid2userpid.count(userid))
    {
        message.set_result("你不在该房间");
    }
    else
    {
        message.set_roomid(m_id);
        message.set_ret(true);
        message.set_result("退出成功");

        userpid_pool.insert(userid2userpid[userid]);
        userid2userpid.erase(userid);

        if (m_gameStarted) NotifyUserQuitGame(userid);

        if (host_userid == userid)
        {
            if (userid2userpid.size() == 0) ChangeRoomHost(-1);
            else ChangeRoomHost(userid2userpid.begin()->first);
        }
    }

    LOGICSERVER.SendToClient(BODYTYPE::LeaveRoom, &message, userid);
}

void Room::KickAllUser()
{
    for(auto p : userid2userpid)
    {
        LeaveRoom(p.first);
    }
}

void Room::ChangeRoomHost(int userid)
{
    if (userid == -1)
    {
        host_userid = -1;
        return;
    }
    if (!userid2userpid.count(userid)) return;
    host_userid = userid;
    // todo 发送房主切换的信息
}

void Room::OnUserOperate(Message *message)
{
    if (!m_gameStarted) return;

    auto op = dynamic_cast<FrameProto::UserOperate *>(message->body->message);
    if (op == nullptr) return;

    int userid = message->head->m_userid;
    if (!userid2userpid.count(userid)) return;
    op->set_userpid(userid2userpid[userid]);
    frame.add_operates()->CopyFrom(*op);
}

void Room::StartGame(int userid)
{
    if (m_gameStarted || userid != host_userid) return;

    m_gameStarted = true;
    frame_id = 0;
    frame.clear_operates();
    all_frames.clear();

    NotifyGameStart();
}

void Room::EndGame()
{
    if ((!m_gameStarted)) return;

    m_gameStarted = false;

    NotifyGameEnd();
}

void Room::Tick() // 有很大的问题吧
{
    if (!m_gameStarted) return;
    frame_id++;
    frame.set_frame_id(frame_id);
    BroadCastToAllClients(BODYTYPE::Frame, &frame);
    all_frames.push_back(frame);
    frame.clear_operates();
}

void Room::BroadCastToAllClients(BODYTYPE type, google::protobuf::MessageLite *message)
{
    for(auto p : userid2userpid)
    {
        LOGICSERVER.SendToClient(type, message, p.first);
    }
}

void Room::NotifyGameStart()
{
    FrameProto::StartGame startGame;
    startGame.set_roomid(m_id);
    startGame.set_seed(m_seed);
    for(auto p : userid2userpid)
    {
        startGame.set_userpid(p.second);
        LOGICSERVER.SendToClient(BODYTYPE::StartGame, &startGame, p.first);
        NotifyUserJoinGame(p.first);
    }
}

void Room::NotifyGameEnd()
{
    FrameProto::CloseGame closeGame;
    closeGame.set_roomid(m_id);
    BroadCastToAllClients(BODYTYPE::CloseGame, &closeGame);
}

void Room::NotifyUserJoinGame(int userid)
{
    if (!m_gameStarted || !userid2userpid.count(userid)) return;

    FrameProto::UserOperate joinGame;
    joinGame.set_userpid(userid2userpid[userid]);
    joinGame.set_opt(FrameProto::OperateType::JoinGame);
    frame.add_operates()->CopyFrom(joinGame);
}

void Room::NotifyUserQuitGame(int userid)
{
    if (!m_gameStarted || !userid2userpid.count(userid)) return;

    FrameProto::UserOperate quitGame;
    quitGame.set_userpid(userid2userpid[userid]);
    quitGame.set_opt(FrameProto::OperateType::LeaveGame);
    frame.add_operates()->CopyFrom(quitGame);
}