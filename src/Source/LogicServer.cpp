#include "../Header/LogicServer.h"

LogicServer::LogicServer()
{
    server_type = SERVER_TYPE::LOGIC;

    room = new Room();
}

LogicServer::~LogicServer()
{
}

bool LogicServer::SendMsg(Message *msg, int fd)
{
    ServerBase::SendMsg(msg, fd);
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
    room->BroadCastMsg();
}

void LogicServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);

    switch (msg->head->m_packageType)
    {
    case BODYTYPE::JoinRoom:
    {
        room->JoinRoom(msg, fd);

        break;
    }
    case BODYTYPE::LeaveRoom:
    {
        room->LeaveRoom(msg, fd);

        break;
    }
    case BODYTYPE::CreateRoom:
    {
        room->CreateRoom(msg, fd);

        break;
    }
    case BODYTYPE::GetRoomList:
    {
        room->GetRoomList(msg, fd);
        
        break;
    }
    case BODYTYPE::StartGame:
    {
        room->NotifyRoomStart(msg, fd);

        break;
    }
    case BODYTYPE::UserOperate:
    {
        room->AddUserOperate(msg, fd);

        break;
    }
    case BODYTYPE::UserInfo:
    {
        ServerProto::UserInfo *body = reinterpret_cast<ServerProto::UserInfo *>(msg->body->message);
        if (body->opt() == ServerProto::UserInfo_Operation::UserInfo_Operation_Logout)
        {
            room->UserLogout(body);
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

    std::cout << "Start Logic Center Server ing..." << std::endl;
    LOGICSERVER.BootServer(port);

    return 0;
}
