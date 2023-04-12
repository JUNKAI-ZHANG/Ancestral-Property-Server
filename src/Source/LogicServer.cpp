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

void LogicServer::BootServer(int port)
{
    ServerBase::BootServer(port);
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
                user_gate[msg->head->m_userid] = fd;
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

        user_gate[msg->head->m_userid] = fd;
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

        break;
    }
    case BODYTYPE::UserInfo:
    {
        ServerProto::UserInfo *body = reinterpret_cast<ServerProto::UserInfo *>(msg->body->message);
        if (body->opt() == ServerProto::UserInfo_Operation::UserInfo_Operation_Logout)
        {
            int userid = body->userid();

            int roomid = user_room[userid];
            if (room[roomid].count(userid))
            {
                room[roomid].erase(userid);
                // std::cout << "user delete" << std::endl;
                if (room[roomid].empty()) 
                {
                    room.erase(roomid);
                    // std::cout << "room delete" << std::endl;

                    user_room.erase(userid);
                    // std::cout << "user_room delete" << std::endl;

                    room_name.erase(roomid);
                    // std::cout << "roomname delete" << std::endl;

                    room_total_frame.erase(roomid);
                    // std::cout << "room_total_frame delete" << std::endl;
                    
                    room_frame.erase(roomid);
                    // std::cout << "room_frame delete" << std::endl;

                    user_gate.erase(userid);
                    // std::cout << "user_gate delete" << std::endl;
                    
                    userid_userpid.erase(userid);
                    // std::cout << "userid_userpid delete" << std::endl;
                    
                    room_framecount.erase(roomid);
                    // std::cout << "room_framecount delete" << std::endl;
                    
                    broadcast_list.erase(roomid);
                    // std::cout << "room remove from broadcast_list" << std::endl;
                }
            }
            else 
            {
                std::cout << "user not in room" << std::endl;
            }

        }
        else if (body->opt() == ServerProto::UserInfo_Operation::UserInfo_Operation_Register)
        {
            
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

    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);
}

void LogicServer::NotifyUserJoin(int userid)
{
    for (int _userid : room[user_room[userid]])
    {
        Message *msg = NewUserJoinRoomMessage(BODYTYPE::EnterGame, userid_userpid[userid], _userid);
        SendMsg(msg, user_gate[_userid]);
        delete msg;
    }
}

void LogicServer::NotifyRoom(BODYTYPE bodytype, int roomid, int fd)
{
    // 1p2p
    int id = 0;
    for (int _userid : room[roomid])
    {
        ++id;
        userid_userpid[_userid] = id;
        Message *msg = NewStartOrCloseGameMessage(bodytype, _userid, roomid, id);
        SendMsg(msg, fd);
        delete msg;

        NotifyUserJoin(_userid);

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
