#include "../Logic/LogicServer.h"
#include "../Logic/Room.h"

LogicServer::LogicServer() 
{
    server_type = SERVER_TYPE::LOGIC;

    // 初始化RoomidPool
    for (int i = 1; i <= ROOM_POOL_SIZE; i++) roomid_pool.insert(i); 

    // 注册广播事件
    Timer *timer = new Timer(ONE_FRAME_MS, CallbackType::LogicServer_Update, std::bind(&LogicServer::Update, this));
    timer->Start();

    m_callfuncList.push_back(timer);
}

LogicServer::~LogicServer()
{
}

bool LogicServer::SendMsg(Message *msg, int fd)
{
    return ServerBase::SendMsg(msg, fd);
}

bool LogicServer::SendToClient(BODYTYPE type, google::protobuf::MessageLite *message, int userid)
{
    if (!user_gate.count(userid))
    {
        return false;
    }
    Message msg;
    MessageBody body;
    body.message = message;
    msg.body = &body;
    MessageHead head(msg.length(), type, userid);
    msg.head = &head;
    bool ret = SendMsg(&msg, user_gate[userid]);
    // 防止析构时出现问题
    msg.body->message = nullptr;
    msg.body = nullptr;
    msg.head = nullptr;
    return ret;
}

void LogicServer::CloseClientSocket(int fd)
{
    ServerBase::CloseClientSocket(fd);

    // 如果和db server断开连接 则尝试重连
    if (fd == db_server_client)
    {
        db_server_client = -1;
        TryToConnectAvailabeServer();
    }
}

void LogicServer::OnConnectToCenterServer()
{
    TryToConnectAvailabeServer();
}

void LogicServer::BootServer(int port)
{
    ServerBase::BootServer(port);
}

void LogicServer::Update()
{
    for (auto room : rooms)
    {
        room.second->Tick();
    }
}

void LogicServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);

    // 因为现在logic收到的消息全部来自于gate
    user_gate[msg->head->m_userid] = fd;

    switch (msg->head->m_packageType)
    {
    case BODYTYPE::LoginResponse:
    {
        LoginProto::LoginResponse *body = dynamic_cast<LoginProto::LoginResponse *>(msg->body->message);
        if (body == nullptr) return;

        // 添加username 到内存
        userid2username[body->userid()] = body->username();

        // 如果玩家还在房间，那么需要发送重连响应回去
        if (userid2roomid.count(body->userid()))
        {
            HandleReconnect(body->userid());
        }
        break;
    }
    case BODYTYPE::JoinRoom:
    {
        HandleJoinRoom(msg);
        break;
    }
    case BODYTYPE::LeaveRoom:
    {
        HandleLeaveRoom(msg);
        break;
    }
    case BODYTYPE::CreateRoom:
    {
        HandleCreateRoom(msg);
        break;
    }
    case BODYTYPE::GetRoomList:
    {
        HandleGetRoomList(msg);
        break;
    }
    case BODYTYPE::StartGame:
    {
        HandleStartGame(msg);
        break;
    }
    case BODYTYPE::EndGame:
    {
        HandleEndGame(msg);
        break;
    }
    case BODYTYPE::UserOperate:
    {
        HandleUserOperate(msg);
        break;
    }
    case BODYTYPE::UserInfo:
    {
        ServerProto::UserInfo *body = dynamic_cast<ServerProto::UserInfo *>(msg->body->message);

        int userid = body->userid();
        if (body->opt() == ServerProto::UserInfo_Operation::UserInfo_Operation_Logout)
        {
            if (userid2roomid.count(userid))
            {
                int roomid = userid2roomid[userid];
                rooms[roomid]->SetUserOffline(userid);
                if (rooms[roomid]->CheckRoomDead())
                {
                    RemoveRoom(roomid);
                }
            }
        }
        else if (body->opt() == ServerProto::UserInfo_Operation::UserInfo_Operation_Register)
        {
            // 暂时没有功能需要实现
        }
        else 
        {
            std::cout << "Error Server MessageType" << std::endl;
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

void LogicServer::TryToConnectAvailabeServer()
{
    if (db_server_client == -1)
    {
        ApplyServerByType(SERVER_TYPE::DATABASE);
    }
}

// 下面的函数是一些业务处理函数，将逻辑与消息发送分开处理
void LogicServer::HandleJoinRoom(Message *msg)
{
    auto body = dynamic_cast<RoomProto::JoinRoom *>(msg->body->message);
    if (body == nullptr) return;

    if (!rooms.count(body->roomid()))
    {
        RoomProto::JoinRoom joinRoom;
        joinRoom.set_roomid(-1);
        joinRoom.set_result("找不到该房间");
        joinRoom.set_type(RoomProto::JoinRoom_Type_RESPONSE);
        joinRoom.set_ret(false);

        SendToClient(BODYTYPE::JoinRoom, &joinRoom, msg->head->m_userid);
        return;
    }
    MovePlayerToRoom(msg->head->m_userid, body->roomid());
}

void LogicServer::HandleLeaveRoom(Message *msg)
{
    auto body = dynamic_cast<RoomProto::LeaveRoom *>(msg->body->message);
    if (body == nullptr) return;

    int userid = msg->head->m_userid;

    if (!rooms.count(body->roomid()) || !userid2roomid.count(userid) || userid2roomid[userid] != body->roomid())
    {
        RoomProto::LeaveRoom leaveRoom;
        leaveRoom.set_roomid(body->roomid());
        leaveRoom.set_ret(false);
        leaveRoom.set_result("你不在该房间");
        leaveRoom.set_type(RoomProto::LeaveRoom_Type_RESPONSE);
        leaveRoom.set_userid(-1);
        leaveRoom.set_userpid(-1);

        SendToClient(BODYTYPE::LeaveRoom, &leaveRoom, msg->head->m_userid);

        return;
    }

    RemovePlayerFromRoom(userid);
}

void LogicServer::HandleCreateRoom(Message *msg)
{
    auto body = dynamic_cast<RoomProto::CreateRoom *>(msg->body->message);
    if (body == nullptr) return;

    RoomProto::CreateRoom createRoom;

    if (userid2roomid.count(msg->head->m_userid))
    {
        createRoom.set_roomid(-1);
        createRoom.set_ret(false);
        createRoom.set_result("你已在房间中");
        createRoom.set_roomname("Error");
        createRoom.set_type(RoomProto::CreateRoom_Type_RESPONSE);
        createRoom.set_is_roomhost(false);
        SendToClient(BODYTYPE::CreateRoom, &createRoom, msg->head->m_userid);
    }
    else
    {
        int roomid = AddRoom(body->roomname());
        createRoom.set_roomid(roomid);
        createRoom.set_ret(true);
        createRoom.set_result("创建成功");
        createRoom.set_roomname(rooms[roomid]->Name());
        createRoom.set_type(RoomProto::CreateRoom_Type_RESPONSE);
        createRoom.set_is_roomhost(true);
        SendToClient(BODYTYPE::CreateRoom, &createRoom, msg->head->m_userid); 
        MovePlayerToRoom(msg->head->m_userid, roomid);
    }
}

void LogicServer::HandleGetRoomList(Message *msg)
{
    RoomProto::GetRoomList getRoomList;
    getRoomList.set_size(roomid_using.size());
    getRoomList.set_ret(true);
    getRoomList.set_type(RoomProto::GetRoomList_Type_RESPONSE);
    for (auto &it : rooms)
    {
        RoomProto::RoomInfo body;
        body.set_roomid(it.first);
        body.set_roomname(it.second->Name());
        body.set_people_count(it.second->PlayerCount());
        getRoomList.add_room_list()->CopyFrom(body);
    }
    SendToClient(BODYTYPE::GetRoomList, &getRoomList, msg->head->m_userid);
}

void LogicServer::MovePlayerToRoom(int userid, int roomid)
{
    RemovePlayerFromRoom(userid);
    AddPlayerToRoom(userid, roomid);
}

void LogicServer::RemovePlayerFromRoom(int userid)
{
    if (!userid2roomid.count(userid)) return;
    int roomid = userid2roomid[userid];
    userid2roomid.erase(userid);

    rooms[roomid]->LeaveFromRoom(userid);

    if (rooms[roomid]->PlayerCount() == 0) RemoveRoom(roomid);
}

void LogicServer::AddPlayerToRoom(int userid, int roomid)
{
    userid2roomid[userid] = roomid;
    rooms[roomid]->JoinRoom(userid);
}

int LogicServer::AddRoom(std::string roomname)
{
    if(!roomid_pool.empty()) {
        int roomid = *roomid_pool.begin();
        roomid_pool.erase(roomid);
        roomid_using.insert(roomid);
        rooms[roomid] = new Room(roomid, roomname, 10);
        return roomid;
    }
    int pooledMax = *roomid_pool.rend();
    int usingMax = 0;
    if (!roomid_using.empty()) usingMax = *roomid_using.rend();
    int roomid = std::max(pooledMax, usingMax) + 1;
    roomid_using.insert(roomid);
    rooms[roomid] = new Room(roomid, roomname, 10);
    return roomid;
}

void LogicServer::RemoveRoom(int roomid)
{
    if (!roomid_using.count(roomid)) return;
    roomid_using.erase(roomid);
    auto itt = userid2roomid;
    for (const auto & p : itt)
    {
        if (p.second == roomid)
        {
            userid2roomid.erase(p.first);
        }
    }
    delete rooms[roomid];
    rooms.erase(roomid);
    if (roomid <= DEFAULT_ROOM_COUNT) roomid_pool.insert(roomid);
}

void LogicServer::HandleStartGame(Message *msg)
{
    if (!CheckUserInRoom(msg->head->m_userid)) return;
    rooms[userid2roomid[msg->head->m_userid]]->StartGame(msg->head->m_userid);
}

void LogicServer::HandleEndGame(Message *msg)
{
    if (!CheckUserInRoom(msg->head->m_userid)) return;
    rooms[userid2roomid[msg->head->m_userid]]->EndGame();
}

void LogicServer::HandleUserOperate(Message *msg)
{
    if (!CheckUserInRoom(msg->head->m_userid)) return;
    rooms[userid2roomid[msg->head->m_userid]]->OnUserOperate(msg);
}

void LogicServer::HandleReconnect(int userid)
{
    FrameProto::Reconnect reconnect;
    if (!userid2roomid.count(userid)) return;
    int roomid = userid2roomid[userid];
    if (!rooms.count(roomid)) return;
    rooms[roomid]->Reconnect(userid);
}

std::string LogicServer::GetUserName(int userid)
{ 
    return userid2username[userid]; 
}

void LogicServer::RemoveUser(int userid)
{
    if (userid2roomid.count(userid))
    {
        userid2roomid.erase(userid);
    }
}

bool LogicServer::CheckUserInRoom(int userid)
{
    if (!userid2roomid.count(userid)) return false;
    if (!rooms.count(userid2roomid[userid])) return false;
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        // printf("Usage: %s port\n", argv[0]);
        //  return 1;
    }

    // int port = std::atoi(argv[1]);
    int port = LOGIC_SERVER_PORT_1;

    std::cout << "Start Logic Center Server ing..." << std::endl;
    LOGICSERVER.BootServer(port);

    return 0;
}
