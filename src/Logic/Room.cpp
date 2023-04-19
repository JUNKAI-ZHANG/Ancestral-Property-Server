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
    EndGame(); // 发送结束游戏回到房间
    KickAllUserFromRoom(); // 踢,并发送退出房间
}

void Room::Resize(int size)
{
    if (size < m_size)
    {
       for (int i = size + 1; i <= m_size; i++)
       {
           Leave(i);
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

void Room::JoinGame(int userid)
{
    if (!m_gameStarted || !userid2userpid.count(userid)) return;
    NotifyUserJoinGame(userid);
    in_game_id2pid[userid] = userid2userpid[userid];
}

void Room::JoinRoom(int userid)
{
    RoomProto::JoinRoom message;
    message.set_type(RoomProto::JoinRoom_Type_RESPONSE);
    message.set_ret(false);
    message.set_roomid(-1);
    if (m_gameStarted == true)
    {
        message.set_result("游戏已经开始，加入失败");
        LOGICSERVER.SendToClient(BODYTYPE::JoinRoom, &message, userid);
    }
    else if (IsFull())
    {
        message.set_result("房间已满");
        LOGICSERVER.SendToClient(BODYTYPE::JoinRoom, &message, userid);
    }
    else if (userid2userpid.count(userid))
    {
        message.set_result("你已在此房间");
        LOGICSERVER.SendToClient(BODYTYPE::JoinRoom, &message, userid);
    }
    else
    {
        if (userpid_pool.empty())
        {
            Resize(m_size + 1);
        }
        int pid = *userpid_pool.begin();
        userpid_pool.erase(pid);

        userid2userpid[userid] = pid;
        id2name[userid] = LOGICSERVER.GetUserName(userid);

        message.set_ret(true);
        message.set_roomid(m_id);
        message.set_result("加入成功");
        auto infos = GetRoomUserInfos();
        message.mutable_users()->Add(infos.begin(), infos.end());

        if (host_userid == -1) ChangeRoomHost(userid);

        BroadCastToRoom(BODYTYPE::JoinRoom, &message); // zjk
    }

    // LOGICSERVER.SendToClient(BODYTYPE::JoinRoom, &message, userid);
}

void Room::Reconnect(int userid)
{
    if (!userid2userpid.count(userid)) return;
    SetUserOnline(userid);

    FrameProto::Reconnect reconnect;
    
    reconnect.set_ret(true);
    reconnect.set_msg("重连成功，已回到房间");
    reconnect.set_userpid(userid2userpid[userid]);
    reconnect.set_roomid(m_id);
    reconnect.set_is_roomhost(host_userid == userid);
    reconnect.set_seed(m_seed);
    
    LOGICSERVER.SendToClient(BODYTYPE::Reconnect, &reconnect, userid);

    NotifyGameStartToUser(userid);
    FrameProto::GameReplay chaseFrame;
    chaseFrame.mutable_frames()->Add(all_frames.begin(), all_frames.end());

    LOGICSERVER.SendToClient(BODYTYPE::ChaseFrame, &chaseFrame, userid);
    JoinGame(userid);
}

void Room::Leave(int userid)
{
    // 把user从所有相关的地方Delete
    LeaveFromGame(userid);
    LeaveFromRoom(userid);
}

void Room::LeaveFromRoom(int userid)
{
    LeaveFromGame(userid);
    RoomProto::LeaveRoom message;
    message.set_type(RoomProto::LeaveRoom_Type_RESPONSE);
    message.set_ret(false);
    message.set_roomid(0);
    message.set_userid(0);
    message.set_userpid(0);
    if (!userid2userpid.count(userid))
    {
        message.set_result("你不在该房间");
        LOGICSERVER.SendToClient(BODYTYPE::LeaveRoom, &message, userid); // 失败了P2P send
    }
    else
    {
        message.set_roomid(m_id);
        message.set_ret(true);
        message.set_userid(userid);
        message.set_userpid(userid2userpid[userid]);
        message.set_result("已退出房间");

        BroadCastToRoom(BODYTYPE::LeaveRoom, &message); // zjk : 先广播再删除

        userpid_pool.insert(userid2userpid[userid]);
        userid2userpid.erase(userid); 
        id2name.erase(userid);

        if (host_userid == userid)
        {
            if (userid2userpid.size() == 0) ChangeRoomHost(-1);
            else ChangeRoomHost(userid2userpid.begin()->first);
        }
    }
}

void Room::LeaveFromGame(int userid)
{
    if (!in_game_id2pid.count(userid)) return;

    NotifyUserQuitGame(userid);
    in_game_id2pid.erase(userid);

    if (in_game_id2pid.empty()) EndGame();
}

void Room::KickAllUserFromGame()
{
    auto tmp = in_game_id2pid;

    for (auto p : tmp)
    {
        LeaveFromGame(p.first);
    }
}

void Room::KickAllUserFromRoom()
{
    auto tmp = userid2userpid;

    for(auto p : tmp)
    {
        LeaveFromRoom(p.first);
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
    frame.add_operates()->CopyFrom(*op); // Even Seg
}

void Room::StartGame(int userid)
{
    if (m_gameStarted || userid != host_userid) return;

    m_gameStarted = true;
    frame_id = 0;
    frame.clear_operates();
    all_frames.clear();

    NotifyGameStart();
    
    for (auto p : userid2userpid)
    {
        JoinGame(p.first);
    }
}

void Room::EndGame()
{
    if ((!m_gameStarted)) return;

    m_gameStarted = false;

    NotifyGameEnd();

    KickAllUserFromGame();
}

void Room::Tick()
{
    if (!m_gameStarted) return;
    frame_id++;
    frame.set_frame_id(frame_id);
    BroadCastToGame(BODYTYPE::Frame, &frame);
    all_frames.push_back(frame);
    frame.clear_operates();
}

void Room::BroadCastToRoom(BODYTYPE type, google::protobuf::MessageLite *message)
{
    for(auto p : userid2userpid)
    {
        LOGICSERVER.SendToClient(type, message, p.first);
    }
}

void Room::BroadCastToGame(BODYTYPE type, google::protobuf::MessageLite *message)
{
    for(auto p : in_game_id2pid)
    {
        LOGICSERVER.SendToClient(type, message, p.first);
    }
}

void Room::NotifyGameStartToUser(int userid)
{
    FrameProto::StartGame startGame;
    startGame.set_roomid(m_id);
    startGame.set_seed(m_seed);
    startGame.set_userpid(userid2userpid[userid]);
    LOGICSERVER.SendToClient(BODYTYPE::StartGame, &startGame, userid);
}

void Room::NotifyGameStart()
{
    for(auto p : userid2userpid)
    {
        NotifyGameStartToUser(p.first);
    }
}

void Room::NotifyGameEnd()
{
    FrameProto::EndGame endGame;
    endGame.set_roomid(m_id);
    BroadCastToGame(BODYTYPE::EndGame, &endGame);
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
    if (!m_gameStarted || !in_game_id2pid.count(userid)) return;

    FrameProto::UserOperate quitGame;
    quitGame.set_userpid(in_game_id2pid[userid]);
    quitGame.set_opt(FrameProto::OperateType::LeaveGame);

    frame.add_operates()->CopyFrom(quitGame);
}

std::vector<RoomProto::UserInfo> Room::GetRoomUserInfos()
{
    std::vector<RoomProto::UserInfo> userInfo(userid2userpid.size());
    int x = 0;
    for (const auto & p : id2name)
    {
        userInfo[x].set_userid(p.first);
        userInfo[x].set_username(p.second);
        x++;
    }
    return userInfo;
}

bool Room::CheckRoomDead()
{
    if (offline_userid.size() == userid2userpid.size())
    {
        return true;
    }
    return false;
}

void Room::SetUserOffline(int userid)
{
    if (!userid2userpid.count(userid) || offline_userid.count(userid)) return;
    offline_userid.insert(userid);
    LeaveFromGame(userid);
}

void Room::SetUserOnline(int userid)
{
    if (!userid2userpid.count(userid) || !offline_userid.count(userid)) return;
    offline_userid.erase(userid);
}