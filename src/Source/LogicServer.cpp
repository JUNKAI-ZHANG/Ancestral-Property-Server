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

void LogicServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);
    Message *message;
    switch (msg->head->m_packageType)
    {
    case BODYTYPE::JoinRoom:
    {
        RoomProto::JoinRoom *body = reinterpret_cast<RoomProto::JoinRoom *>(msg->body->message);
        if (room.count(body->roomid()))
        {
            if (room[body->roomid()].size() == 1)
            {
                room[body->roomid()].insert(body->userid());
                message = NewJoinRoomResponse(body->userid(), "Join success!", true);
                SendMsg(message, fd);
            }
            else
            {
                message = NewJoinRoomResponse(body->userid(), "Full of people...", false);
                SendMsg(message, fd);
            }
        }
        else
        {
            message = NewJoinRoomResponse(body->userid(), "Room is not exist...", false);
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
                _room.erase(body->userid());
                message = NewLeaveRoomResponse(body->userid(), "Exit success!", true);
                SendMsg(message, fd);
            }
            else if (_room.size() == 1)
            {
                room.erase(body->roomid());
                message = NewLeaveRoomResponse(body->userid(), "Room destroy", true);
                SendMsg(message, fd);
            }
        }
        else
        {
            message = NewLeaveRoomResponse(body->userid(), "Room is not exist...", false);
            SendMsg(message, fd);
        }
        break;
    }
    case BODYTYPE::CreateRoom:
    {
        RoomProto::CreateRoom *body = reinterpret_cast<RoomProto::CreateRoom *>(msg->body->message);

        int room_id = ++now_room_count;
        room[room_id].insert(body->userid());

        message = NewCreateRoomResponse(body->userid(), "Create room success!", true);
        SendMsg(message, fd);

        std::cout << "Userid = " << body->userid() << std::endl;
        std::cout << "Roomid = " << now_room_count << std::endl;

        break;
    }
    case BODYTYPE::GetRoomList:
    {
        RoomProto::GetRoomList *body = reinterpret_cast<RoomProto::GetRoomList *>(msg->body->message);

        int _size = room.size();

        int roomlist[_size], people[_size], p = 0;
        for (const auto &it : room)
        {
            roomlist[p] = it.first;
            people[p++] = it.second.size();
        }

        message = NewGetRoomListResponse(body->userid(), "Get success!", roomlist, people, _size, true);
        SendMsg(message, fd);

        break;
    }
    default:
        break;
    }

    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);
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

    printf("Start Logic Center Server ing...\n");
    logicServer.BootServer(port);

    return 0;
}
