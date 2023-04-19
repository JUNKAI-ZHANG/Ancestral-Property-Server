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
    else if (fd == db_server_client)
    {
        db_server_client = -1;

        TryToConnectAvailabeServer();
    }
    else 
    {
        Message *msg = NewUserInfoMessage(fd_user_record[fd], fd, ServerProto::UserInfo_Operation::UserInfo_Operation_Logout);
        SendMsg(msg, logic_server_client);
        delete msg;
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

bool GateServer::CheckMessageValid(Message *msg, int fd)
{
    if (fd == center_server_client || fd == logic_server_client || fd == db_server_client)
    {
        return true;
    }
    if (msg->head->m_userid == 0)
    {
        int type = msg->head->m_packageType;
        if (type == BODYTYPE::LoginRequest || type == BODYTYPE::RegistRequest)
        {
            return true;
        }
    }
    else 
    {
        if (fd == user_fd_record[msg->head->m_userid])
        {
            return true;
        }
    }
    return false;
}

void GateServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);

    if (!CheckMessageValid(msg, fd))
    {
        return;
    }

    switch (msg->head->m_packageType)
    {
    case BODYTYPE::LoginRequest:
    {
        // 登录的m_userid字段暂时存储user_fd
        msg->head->m_userid = fd;

        // 转发给db server处理
        SendMsg(msg, db_server_client);
        break;
    }
    case BODYTYPE::LoginResponse:
    {
        LoginProto::LoginResponse *body = reinterpret_cast<LoginProto::LoginResponse *>(msg->body->message);

        if (body->result())
        {
            user_fd_record[body->userid()] = msg->head->m_userid;
            fd_user_record[msg->head->m_userid] = body->userid();

            /* 如果成功了，那么让logicServer添加userid-username到内存 */
            SendMsg(msg, logic_server_client);
        }

        SendMsg(msg, msg->head->m_userid);
        break;
    }
    case BODYTYPE::RegistRequest:
    {
        // 注册的m_userid字段暂时存储user_fd
        msg->head->m_userid = fd;

        // 转发给db server处理
        SendMsg(msg, db_server_client);
        break;
    }
    case BODYTYPE::RegistResponse:
    {
        // 无论成功与否，都转发给client，无需额外的操作
        SendMsg(msg, msg->head->m_userid);
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
            std::cout << user_fd_record[msg->head->m_userid] << std::endl;
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
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
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
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
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
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
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
        }
        else 
        {
            std::cerr << "GateServer : Error JoinRoom Type" << std::endl;
        }
        break;
    }
    case BODYTYPE::StartGame:
    {
        if (fd == logic_server_client)
        {
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
        }
        else 
        {
            SendMsg(msg, logic_server_client);
        }
        break;
    }
    case BODYTYPE::UserOperate:
    {
        SendMsg(msg, logic_server_client);
        break;
    }
    case BODYTYPE::Frame:
    {
        SendMsg(msg, user_fd_record[msg->head->m_userid]);
        break;
    }
    case BODYTYPE::Reconnect:
    {
        SendMsg(msg, user_fd_record[msg->head->m_userid]);
        break;
    }
    case BODYTYPE::ChaseFrame:
    {
        SendMsg(msg, user_fd_record[msg->head->m_userid]);
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
        // printf("Usage: %s port\n", argv[0]);
        // return 1;
    }

    // int port = std::atoi(argv[1]);
    int port = GATE_SERVER_PORT_1;

    GateServer gateServer;

    std::cout << "Start Gate Center Server ing..." << std::endl;
    gateServer.BootServer(port);

    return 0;
}
