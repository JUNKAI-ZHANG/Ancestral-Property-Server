#include "../Header/LogicServer.h"

LogicServer::LogicServer()
{
    server_type = SERVER_TYPE::LOGIC;

    StartBroadCastToClient();
}

LogicServer::~LogicServer()
{
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

void LogicServer::StartBroadCastToClient()
{
    Timer timer;
    timer.start(33, &LogicServer::BroadCastMsg, this);
}

void LogicServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);
    Message *message;
    int userid = msg->head->m_userid;
    switch (msg->head->m_packageType)
    {
    case BODYTYPE::JoinRoom:
    {
        RoomProto::JoinRoom *body = reinterpret_cast<RoomProto::JoinRoom *>(msg->body->message);
        if (user_room.count(userid))
        {
            message = NewJoinRoomResponse("Already in room " + to_string(user_room[userid]), false, userid);
            SendMsg(message, fd);
        }
        else if (room.count(body->roomid()))
        {
            if (room[body->roomid()].size() == 1)
            {
                room_mutex.lock();
                room[body->roomid()].insert(userid);
                broadcast_list.insert(body->roomid());
                user_room[userid] = body->roomid();
                room_mutex.unlock();

                message = NewJoinRoomResponse("Join success!", true, userid);
                SendMsg(message, fd);
                // StartGame 
                NotifyRoom(BODYTYPE::StartGame, body->roomid(), fd);
            }
            else
            {
                message = NewJoinRoomResponse("Full of people...", false, userid);
                SendMsg(message, fd);
            }
        }
        else
        {
            message = NewJoinRoomResponse("Room is not exist...", false, userid);
            SendMsg(message, fd);
        }
        break;
    }
    case BODYTYPE::LeaveRoom:
    {
        RoomProto::LeaveRoom *body = reinterpret_cast<RoomProto::LeaveRoom *>(msg->body->message);
        if (room.count(body->roomid()))
        {
            auto _room = room[body->roomid()];
            if (_room.size() == 2)
            {
                room_mutex.lock();
                _room.erase(userid);
                user_room.erase(userid);
                room_mutex.unlock();

                message = NewLeaveRoomResponse("Exit success!", true, userid);
                SendMsg(message, fd);
            }
            else if (_room.size() == 1)
            {
                room_mutex.lock();
                user_room.erase(userid);
                room.erase(body->roomid());
                room_mutex.unlock();

                message = NewLeaveRoomResponse("Exit success, Room destroy", true, userid);
                SendMsg(message, fd);
            }
        }
        else
        {
            message = NewLeaveRoomResponse("Room is not exist...", false, userid);
            SendMsg(message, fd);
        }
        break;
    }
    case BODYTYPE::CreateRoom:
    {
        RoomProto::CreateRoom *body = reinterpret_cast<RoomProto::CreateRoom *>(msg->body->message);

        int room_id = ++now_room_count;

        room_mutex.lock();
        room[room_id].insert(userid);
        user_room[userid] = room_id;
        room_name[room_id] = body->roomname();
        room_mutex.unlock();

        message = NewCreateRoomResponse("Create room success!", room_id, body->roomname(), true, userid);
        SendMsg(message, fd);

        break;
    }
    case BODYTYPE::GetRoomList:
    {
        RoomProto::GetRoomList *body = reinterpret_cast<RoomProto::GetRoomList *>(msg->body->message);

        int _size = room.size();

        int roomlist[_size], people[_size], p = 0;
        std::string roomname[_size];
        for (const auto &it : room)
        {
            roomlist[p] = it.first;
            people[p] = it.second.size();
            roomname[p++] = room_name[it.first];
        }

        message = NewGetRoomListResponse("Get success!", roomlist, people, roomname, _size, true, userid);
        SendMsg(message, fd);

        break;
    }
    case BODYTYPE::Frame:
    {
        if (!user_gate.count(msg->head->m_userid))
        {
            user_gate[msg->head->m_userid] = fd;
        }
        user_now_frame[msg->head->m_userid].push(msg);
        break;
    }
    default:
    {
        break;
    }
    }

    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);
}

void LogicServer::NotifyRoom(BODYTYPE bodytype, int roomid, int fd)
{
    // 1p2p
    for (int userid : room[roomid])
    {
        Message *msg = NewStartOrCloseGameMessage(bodytype, roomid, userid);
        SendMsg(msg, fd);
        delete msg;
    }
}

void LogicServer::BroadCastMsg()
{
    for (int _room : broadcast_list)
    {
        for (int _userid : room[_room])
        {
            auto iter = user_now_frame[_userid];
            if (!iter.empty())
            {
                Message *tmp = iter.front();
                int fd = user_gate[tmp->head->m_userid];
                for (int __userid : room[_room])
                {
                    SendMsg(tmp, fd);
                }
            }
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        // printf("Usage: %s port\n", argv[0]);
        //  return 1;
    }

    // int port = std::atoi(argv[1]);
    int port = 10809;

    LogicServer logicServer;

    std::cout << "Start Logic Center Server ing..." << std::endl;
    logicServer.BootServer(port);

    return 0;
}
