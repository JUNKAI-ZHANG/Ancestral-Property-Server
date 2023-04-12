#include "../Header/LogicServer.h"

LogicServer::LogicServer()
{
    server_type = SERVER_TYPE::LOGIC;
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

void LogicServer::BroadCastMsg()
{
    for (int _room : broadcast_list)
    {
        std::cout << 123 << std::endl;
        room_framecount[_room]++;

        // bcast.lock();
        Message *msg = new Message;
        msg->body = new MessageBody;
        FrameProto::Frame *body = new FrameProto::Frame;
        while (!room_frame[_room].empty())
        {
            Message *tmp = room_frame[_room].front();
            room_frame[_room].pop();

            FrameProto::UserOperate *tmP = reinterpret_cast<FrameProto::UserOperate *>(tmp->body->message);

            FrameProto::UserOperate *user_data = body->add_operates();
            
            for (int64_t i : tmP->data())
            {
                user_data->add_data(i);
            }
            user_data->set_opt(tmP->opt());

        }
        body->set_frame_id(room_framecount[_room]);
        msg->body->message = body;

        msg->head = new MessageHead(msg->length(), BODYTYPE::Frame, 0);

        for (int _userid : room[_room])
        {
            msg->head->m_userid = _userid;
            int fd = user_gate[_userid];
            ServerBase::SendMsg(msg, fd);
        }
        if (msg != nullptr) 
        {
            delete msg;
        }
        // bcast.unlock();
    }
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
                room[body->roomid()].insert(userid);
                user_room[userid] = body->roomid();

                message = NewJoinRoomResponse("Join success!", true, userid);
                SendMsg(message, fd);
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
                _room.erase(userid);
                user_room.erase(userid);

                message = NewLeaveRoomResponse("Exit success!", true, userid);
                SendMsg(message, fd);
            }
            else if (_room.size() == 1)
            {
                user_room.erase(userid);
                room.erase(body->roomid());

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

        room[room_id].insert(userid);
        user_room[userid] = room_id;
        room_name[room_id] = body->roomname();

        message = NewCreateRoomResponse("Create room success!", room_id, body->roomname(), true, userid, true);
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
    case BODYTYPE::StartGame:
    {
        FrameProto::StartGame *body = reinterpret_cast<FrameProto::StartGame *>(msg->body->message);
        // StartGame
        broadcast_list.insert(body->roomid());
        NotifyRoom(BODYTYPE::StartGame, body->roomid(), fd);

        break;
    }
    case BODYTYPE::UserOperate:
    {
        user_gate[msg->head->m_userid] = fd;
        int roomid = user_room[msg->head->m_userid];
        room_frame[roomid].push(msg);
        room_total_frame[roomid].push_back(msg);

        FrameProto::UserOperate *body = reinterpret_cast<FrameProto::UserOperate *>(msg->body->message);


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
    int id = 0;
    for (int userid : room[roomid])
    {
        ++id;
        userid_userpid[userid] = id;
        Message *msg = NewStartOrCloseGameMessage(bodytype, userid, roomid, id);
        SendMsg(msg, fd);
        delete msg;
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
