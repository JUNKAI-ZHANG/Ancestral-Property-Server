#include "../Header/GateServer.h"

GateServer::GateServer()
{
    server_type = SERVER_TYPE::GATE;
}

GateServer::~GateServer()
{
}

void GateServer::CloseClientSocket(int fd)
{
    ServerBase::CloseClientSocket(fd);

    // 如果和logic server断开连接 告知所有客户端并则尝试重连
    if (fd == logic_server_client)
    {
        // notice clients
        DisconnectAllClients();

        // retry
        logic_server_client = -1;
        TryToConnectAvailabeServer();
    }

    // 如果和db server断开连接 则尝试重连
    if (fd == db_server_client)
    {
        db_server_client = -1;
        TryToConnectAvailabeServer();
    }
}

void GateServer::OnConnectToCenterServer()
{
    TryToConnectAvailabeServer();
}

void GateServer::TryToConnectAvailabeServer()
{
    if (logic_server_client == -1)
    {
        ApplyServerByType(SERVER_TYPE::LOGIC);
    }

    if (db_server_client == -1)
    {
        ApplyServerByType(SERVER_TYPE::DATABASE);
    }
}

void GateServer::DisconnectAllClients()
{
    for (auto it = connections.begin(); it != connections.end();)
    {
        if (it->first == center_server_client)
        {
            ++it;
        }
        else if (it->first == logic_server_client)
        {
            ++it;
        }
        else if (it->first == db_server_client)
        {
            ++it;
        }
        else
        {
            auto cur = it;
            ++it;
            CloseClientSocket(cur->first);
        }
    }
}

void GateServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);

    switch (msg->head->m_packageType)
    {
    case BODYTYPE::LoginRequest:
    {
        LoginProto::LoginRequest *body1 = reinterpret_cast<LoginProto::LoginRequest *>(msg->body->message);

        // std::cout << "username = " << body1->username() << std::endl;
        // std::cout << "password = " << body1->passwd() << std::endl;

        user_fd_record[body1->username()] = fd;
        
        // 转发给db server处理
        SendMsg(msg, db_server_client);
        break;
    }
    case BODYTYPE::LoginResponse:
    {
        LoginProto::LoginResponse *body2 = reinterpret_cast<LoginProto::LoginResponse *>(msg->body->message);

        // std::cout << "Size = " << msg->head->m_packageSize << std::endl;
        // std::cout << "Type = " << msg->head->m_packageType << std::endl;
        // std::cout << "Token = " << body2->token() << std::endl;

        if (!user_fd_record.count(body2->userid()))
        {
            return;
        }

        int client_fd = user_fd_record[body2->userid()];
        SendMsg(msg, client_fd);

        if (body2->opt() == LoginProto::LoginResponse_Operation_Register)
        {
            // 注册操作 移除临时record
            user_fd_record.erase(body2->userid());
        }
        else if (!(body2->result()))
        {
            // 登录失败 移除record
            user_fd_record.erase(body2->userid());
        }
        break;
    }
    case BODYTYPE::JoinRoom:
    {
        RoomProto::JoinRoom *body = reinterpret_cast<RoomProto::JoinRoom *>(msg->body->message);
        if (body->type() == RoomProto::JoinRoom::Type::JoinRoom_Type_REQUEST)
        {
            /* 转发给logic server处理 */
            SendMsg(msg, logic_server_client);
        }
        else if (body->type() == RoomProto::JoinRoom::Type::JoinRoom_Type_RESPONSE)
        {
            /* 转发给 client 处理 */
            SendMsg(msg, user_fd_record[body->userid()]);
        }
        else 
        {
            std::cerr << "GateServer : Error JoinRoom Type" << std::endl;
        }
        break;
    }
    case BODYTYPE::LeaveRoom:
    {
        RoomProto::LeaveRoom *body = reinterpret_cast<RoomProto::LeaveRoom *>(msg->body->message);
        if (body->type() == RoomProto::LeaveRoom::Type::LeaveRoom_Type_REQUEST)
        {
            /* 转发给logic server处理 */
            SendMsg(msg, logic_server_client);
        }
        else if (body->type() == RoomProto::LeaveRoom::Type::LeaveRoom_Type_RESPONSE)
        {
            /* 转发给 client 处理 */
            SendMsg(msg, user_fd_record[body->userid()]);
        }
        else 
        {
            std::cerr << "GateServer : Error LeaveRoom Type" << std::endl;
        }
        break;
    }
    case BODYTYPE::CreateRoom:
    {
        RoomProto::CreateRoom *body = reinterpret_cast<RoomProto::CreateRoom *>(msg->body->message);
        if (body->type() == RoomProto::CreateRoom::Type::CreateRoom_Type_REQUEST)
        {
            /* 转发给logic server处理 */
            SendMsg(msg, logic_server_client);
        }
        else if (body->type() == RoomProto::CreateRoom::Type::CreateRoom_Type_RESPONSE)
        {
            /* 转发给 client 处理 */
            SendMsg(msg, user_fd_record[body->userid()]);
        }
        else 
        {
            std::cerr << "GateServer : Error CreateRoom Type" << std::endl;
        }
        break;
    }
    case BODYTYPE::GetRoomList:
    {
        RoomProto::GetRoomList *body = reinterpret_cast<RoomProto::GetRoomList *>(msg->body->message);
        if (body->type() == RoomProto::GetRoomList::Type::GetRoomList_Type_REQUEST)
        {
            /* 转发给logic server处理 */
            SendMsg(msg, logic_server_client);
        }
        else if (body->type() == RoomProto::GetRoomList::Type::GetRoomList_Type_RESPONSE)
        {
            /* 转发给 client 处理 */
            SendMsg(msg, user_fd_record[body->userid()]);
        }
        else 
        {
            std::cerr << "GateServer : Error JoinRoom Type" << std::endl;
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s port\n", argv[0]);
        // return 1;
    }

    // int port = std::atoi(argv[1]);
    int port = 10808;

    GateServer gateServer;

    printf("Start Gate Center Server ing...\n");
    gateServer.BootServer(port);

    return 0;
}
